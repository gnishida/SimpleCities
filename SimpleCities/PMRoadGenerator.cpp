#include "PMRoadGenerator.h"
#include <QTest>
#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include "global.h"
#include "Util.h"
#include "GraphUtil.h"
#include <assert.h>
#include "Util.h"

void PMRoadGenerator::generateRoadNetwork() {
	srand(12);

	// make roads along the boundary of the area
	generateRoadsAtBoundary();

	std::list<RoadVertexDesc> seeds;
	std::vector<std::pair<QVector2D, float>> regular_elements;

	// １本の道路をとりあえず作成
	RoadVertexPtr v0 = RoadVertexPtr(new RoadVertex(QVector2D(0, 0)));
	RoadVertexDesc v0_desc = GraphUtil::addVertex(roads, v0);
	roads.graph[v0_desc]->eigenType = RoadVertex::EIGEN_TYPE_MAJOR;
	seeds.push_back(v0_desc);
	float segment_length = G::getFloat("avenueAvgSegmentLength");
	float curvature = G::getFloat("road_curvature");
	growRoads(0.0, v0_desc, curvature, segment_length, RoadVertex::EIGEN_TYPE_MAJOR, regular_elements, seeds);
	growRoads(0.0, v0_desc, curvature, -segment_length, RoadVertex::EIGEN_TYPE_MAJOR, regular_elements, seeds);
	
	// setup the tensor field
	cv::Mat tensor;
	setupTensor(targetArea, regular_elements, tensor);
	//saveTensorImage(tensor, "tensor.png");

	// generate roads
	generateRoadsByTensor(tensor, segment_length, segment_length * 0.7, seeds);

	// remove dangling edges
	removeDanglingEdges();
}

void PMRoadGenerator::generateRoadsAtBoundary() {
	RoadVertexDesc first_desc;
	RoadVertexDesc cur_desc;
	for (int i = 0; i < targetArea.size(); ++i) {
		int next = (i + 1) % targetArea.size();

		// create an intersection
		if (i == 0) {
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(targetArea[i]));
			cur_desc = GraphUtil::addVertex(roads, v);
			first_desc = cur_desc;
		}

		RoadVertexDesc next_desc;
		if (next == 0) {
			next_desc = first_desc;
		}
		else {
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(targetArea[next]));
			next_desc = GraphUtil::addVertex(roads, v);
		}

		// add edge
		RoadEdgePtr e = RoadEdgePtr(new RoadEdge(RoadEdge::TYPE_AVENUE, 1));
		e->polyline.push_back(roads.graph[cur_desc]->pt);
		e->polyline.push_back(roads.graph[next_desc]->pt);
		GraphUtil::addEdge(roads, cur_desc, next_desc, e);

		cur_desc = next_desc;
	}
}

/**
 * 座標pt、角度angleから開始して１本の道路を生成する。
 *
 * @param size						ターゲットエリアの一辺の長さ
 * @param angle						角度
 * @param pt						開始位置
 * @param curvature					曲率
 * @param segment_length			segment length
 * @param regular_elements [OUT]	tensor fieldを指定するためのコントロール情報
 * @param seeds [OUT]				生成された頂点をseedリストに追加する
*/
void PMRoadGenerator::growRoads(float angle, RoadVertexDesc srcDesc, float curvature, float segment_length, int type, std::vector<std::pair<QVector2D, float>>& regular_elements, std::list<RoadVertexDesc>& seeds) {
	int num_steps = 5;

	QVector2D pt = roads.graph[srcDesc]->pt;

	bool loop = true;
	while (loop) {
		// 今後５ステップ分の曲率を、平均がcurvatureになるようランダムに決定する。
		std::vector<float> rotates;
		float total = 0.0f;
		for (int i = 0; i < num_steps; ++i) {
			float r = rand() % 100;
			rotates.push_back(r);
			total += r;
		}
		for (int i = 0; i < num_steps; ++i) {
			rotates[i] = rotates[i] / total * curvature * num_steps;
		}

		// 曲がる方向を決定する
		int rotate_dir = rand() % 2 == 0 ? 1 : -1;

		// 5ステップ分の道路セグメントを生成する
		for (int i = 0; i < num_steps; ++i) {
			// ターゲットエリア外なら終了
			if (!targetArea.contains(pt)) return;

			angle += rotates[i] * rotate_dir;
			QVector2D pt2 = pt + QVector2D(cosf(angle), sinf(angle)) * segment_length;

			RoadEdgeDesc closestEdge;
			QVector2D intPt;
			if (GraphUtil::isIntersect(roads, pt, pt2, closestEdge, intPt)) {
				// 他のエッジにスナップ
				RoadVertexDesc tgtDesc = GraphUtil::splitEdge(roads, closestEdge, intPt);
				roads.graph[tgtDesc]->eigenType |= type;

				// エッジを生成
				RoadEdgePtr new_edge = RoadEdgePtr(new RoadEdge(RoadEdge::TYPE_AVENUE, 1));
				new_edge->addPoint(pt);
				new_edge->addPoint(intPt);
				GraphUtil::addEdge(roads, srcDesc, tgtDesc, new_edge);

				return;
			}		

			// add a vertex
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(pt2));
			RoadVertexDesc tgtDesc = GraphUtil::addVertex(roads, v);
			roads.graph[tgtDesc]->eigenType = type;

			// add this vertex to the seeds
			seeds.push_back(tgtDesc);

			// add an edge
			RoadEdgePtr new_edge = RoadEdgePtr(new RoadEdge(RoadEdge::TYPE_AVENUE, 1));
			new_edge->addPoint(pt);
			new_edge->addPoint(pt2);
			GraphUtil::addEdge(roads, srcDesc, tgtDesc, new_edge);

			regular_elements.push_back(std::make_pair(pt2, angle));

			pt = pt2;
			srcDesc = tgtDesc;
		}
	}
}

void PMRoadGenerator::setupTensor(const Polygon2D& targetArea, const std::vector<std::pair<QVector2D, float>>& regular_elements, cv::Mat& tensor) {
	BBox bbox = targetArea.envelope();
	tensor = cv::Mat(bbox.dy() + 1, bbox.dx() + 1, CV_32F);

	for (int r = 0; r < tensor.rows; ++r) {
		for (int c = 0; c < tensor.cols; ++c) {
			int x = c + bbox.minPt.x();
			int y = r + bbox.minPt.y();

			double total_angle = 0.0;
			double total_weight = 0.0;
			for (int k = 0; k < regular_elements.size(); ++k) {
				float dist = (QVector2D(x, y) - regular_elements[k].first).length();

				double angle = regular_elements[k].second;
				double weight = exp(-dist / 10);
				total_angle += angle * weight;
				total_weight += weight;
			}

			float avg_angle = total_angle / total_weight;
			tensor.at<float>(r, c) = avg_angle;
		}
	}
}

/**
* 指定されたtensor fieldに基づいて道路網を生成する。
*
* @param tensor			tensor field
* @param segment_length	segment length
* @param near_threshold	near threshold
* @param vertices [OUT]	既存交差点
* @param edges	[OUT]	既存セグメント
*/
void PMRoadGenerator::generateRoadsByTensor(const cv::Mat& tensor, float segment_length, float near_threshold, std::list<RoadVertexDesc>& seeds) {
	int count = 0;
	while (!seeds.empty()) {
		RoadVertexDesc desc = seeds.front();
		seeds.pop_front();

		if (roads.graph[desc]->eigenType == RoadVertex::EIGEN_TYPE_NONE) {
			std::cout << "ERROR: eigenType is unknown!!!" << std::endl;
		}

		// ターゲットエリア外ならスキップ
		if (!targetArea.contains(roads.graph[desc]->pt)) continue;

		// このシードから生成する道路のeigen vectorのタイプを決定する
		int type;
		if (roads.graph[desc]->eigenType == RoadVertex::EIGEN_TYPE_MAJOR) {
			type = RoadVertex::EIGEN_TYPE_MINOR;
		}
		else if (roads.graph[desc]->eigenType == RoadVertex::EIGEN_TYPE_MINOR) {
			type = RoadVertex::EIGEN_TYPE_MAJOR;
		}
		else {
			// 既にmajor, minorが交差しているので、これ以上、この交差点から道路を生成しない
			continue;
		}

		// 既に近くに頂点がないかチェック
		bool foundNearVertex = false;
		RoadVertexIter vi, vend;
		for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
			// 自分自身ならスキップ
			if (*vi == desc) continue;

			// major vector と minor vectorで異なる場合は、チェックしない
			if (!(roads.graph[*vi]->eigenType & type)) continue;

			float dist = (roads.graph[*vi]->pt - roads.graph[desc]->pt).length();
			if (dist < near_threshold) {
				foundNearVertex = true;
				break;
			}
		}

		std::cout << count << ": (" << roads.graph[desc]->pt.x() << ", " << roads.graph[desc]->pt.y() << ") " << type << (foundNearVertex ? " canceled" : "") << std::endl;

		if (!foundNearVertex) {
			generateRoadByTensor(tensor, segment_length, near_threshold, desc, type, seeds);
			generateRoadByTensor(tensor, -segment_length, near_threshold, desc, type, seeds);
		}

		count++;
		if (count > 500) break;
	}
}

/**
* 指定されたtensor fieldに基づいて、ptから道路生成を行う。
*
* @param tensor			tensor field
* @param segment_length	segment length
* @param near_threshold	near threshold
* @param pt				この座標から道路セグメント生成を開始する
* @param type			1 -- major eigen vector / 2 -- minor eigen vector
* @param seeds [OUT]	既存交差点
*/
void PMRoadGenerator::generateRoadByTensor(const cv::Mat& tensor, float segment_length, float near_threshold, RoadVertexDesc srcDesc, int type, std::list<RoadVertexDesc>& seeds) {
	QVector2D pt = roads.graph[srcDesc]->pt;

	RoadVertexDesc tgtDesc;

	while (true) {
		int result = generateRoadSegmentByTensor(tensor, segment_length, near_threshold, srcDesc, tgtDesc, type);
		if (result == 1) {	// ターゲットエリア外に出た
			break;
		}
		else if (result == 2) {	// 既存交差点近くのエッジに交差した
			break;
		}
		else {
			// 既に近くに頂点がないかチェック
			bool foundNearVertex = false;
			RoadVertexIter vi, vend;
			for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
				if (*vi == tgtDesc) continue;

				// 現在生成中の道路セグメントは、チェックしない。
				if (roads.graph[*vi]->new_vertx) continue;

				// major vector と minor vectorで異なる場合は、チェックしない
				if (!(roads.graph[*vi]->eigenType & type)) continue;

				float dist = (roads.graph[*vi]->pt - roads.graph[tgtDesc]->pt).length();
				if (dist < near_threshold) {
					foundNearVertex = true;
					break;
				}
			}

			if (foundNearVertex) break;
		}

		seeds.push_back(tgtDesc);

		srcDesc = tgtDesc;
	}

	// 新規交差点の新規フラグをクリアする
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
		roads.graph[*vi]->new_vertx = false;
	}
}

/**
* 指定されたtensor filedに基づいて、ptからsegment_length分の道路セグメントを生成する。
* ただし、ターゲットエリア外に出るか、既存セグメントとの交差点が既存交差点の近くなら、途中で終了する。
*
* @param tensor				tensor field
* @param segment_length		segment length
* @param near_threshold		near threshold
* @param pt [OUT]			この座標から道路セグメント生成を開始する。また、最終地点の座標が格納される。
* @param type				1 -- major eigen vector / 2 -- minor eigen vector
* @param new_vertices [OUT]	既存セグメントとの交差点をこのリストに追加する
* @return					0 -- 正常終了 / 1 -- ターゲットエリア外に出て終了 / 2 -- 既存交差点の近くで交差して終了
*/
int PMRoadGenerator::generateRoadSegmentByTensor(const cv::Mat& tensor, float segment_length, float near_threshold, RoadVertexDesc srcDesc, RoadVertexDesc& tgtDesc, int type) {
	int num_step = 5;
	float step_length = segment_length / num_step;

	BBox bbox = targetArea.envelope();

	QVector2D pt = roads.graph[srcDesc]->pt;
	RoadEdgePtr new_edge = RoadEdgePtr(new RoadEdge(RoadEdge::TYPE_AVENUE, 1));
	new_edge->addPoint(pt);

	for (int i = 0; i < num_step; ++i) {
		// ターゲットエリア外ならストップ
		if (!targetArea.contains(pt)) {
			return 1;
		}

		/////////////////////////////////////////////////////////////////////
		// use Runge-Kutta 4 to obtain the next angle
		int c = pt.x() - bbox.minPt.x();
		int r = pt.y() - bbox.minPt.y();
		float angle1 = tensor.at<float>(r, c);
		if (type == 2) angle1 += M_PI / 2;	// minor eigen vectorならPI/2を足す

		// angle2
		QVector2D pt2 = pt + QVector2D(cosf(angle1), sinf(angle1)) * (step_length * 0.5);
		int c2 = pt2.x() - bbox.minPt.x();
		int r2 = pt2.y() - bbox.minPt.y();
		float angle2 = angle1;
		if (c2 >= 0 && c2 < tensor.cols && r2 >= 0 && r2 < tensor.rows) {
			angle2 = tensor.at<float>(r2, c2);
			if (type == 2) angle2 += M_PI / 2;	// minor eigen vectorならPI/2を足す
		}

		// angle3
		QVector2D pt3 = pt + QVector2D(cosf(angle2), sinf(angle2)) * (step_length * 0.5);
		int c3 = pt3.x() - bbox.minPt.x();
		int r3 = pt3.y() - bbox.minPt.y();
		float angle3 = angle2;
		if (c3 >= 0 && c3 < tensor.cols && r3 >= 0 && r3 < tensor.rows) {
			angle3 = tensor.at<float>(r3, c3);
			if (type == 2) angle3 += M_PI / 2;	// minor eigen vectorならPI/2を足す
		}

		// angle4
		QVector2D pt4 = pt + QVector2D(cosf(angle3), sinf(angle3)) * step_length;
		int c4 = pt4.x() - bbox.minPt.x();
		int r4 = pt4.y() - bbox.minPt.y();
		float angle4 = angle3;
		if (c4 >= 0 && c4 < tensor.cols && r4 >= 0 && r4 < tensor.rows) {
			angle4 = tensor.at<float>(r4, c4);
			if (type == 2) angle4 += M_PI / 2;	// minor eigen vectorならPI/2を足す
		}

		// RK4によりangleを計算
		float angle = angle1 / 6.0 + angle2 / 3.0 + angle3 / 3.0 + angle4 / 6.0;

		// 次のステップの座標を求める
		QVector2D next_pt = pt + QVector2D(cosf(angle), sinf(angle)) * step_length;

		// 交差点を求める
		RoadEdgeDesc nearestEdgeDesc;
		QVector2D intPt;
		if (GraphUtil::isIntersect(roads, pt, next_pt, nearestEdgeDesc, intPt)) {
			// 他のエッジにスナップ
			tgtDesc = GraphUtil::splitEdge(roads, nearestEdgeDesc, intPt);
			roads.graph[tgtDesc]->eigenType |= type;
			roads.graph[tgtDesc]->new_vertx = true;

			new_edge->addPoint(intPt);

			// エッジを生成
			GraphUtil::addEdge(roads, srcDesc, tgtDesc, new_edge);

			// エッジを交差点から再開させる
			srcDesc = tgtDesc;
			pt = intPt;
			new_edge = RoadEdgePtr(new RoadEdge(RoadEdge::TYPE_AVENUE, 1));
			new_edge->addPoint(pt);

			// 既に近くに頂点がないかチェック
			bool foundNearVertex = false;
			RoadVertexIter vi, vend;
			for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
				// 現在生成中の道路セグメントは、チェックしない。
				if (roads.graph[*vi]->new_vertx) continue;

				// major vector と minor vectorで異なる場合は、チェックしない
				if (!(roads.graph[*vi]->eigenType & type)) continue;

				float dist = (roads.graph[*vi]->pt - intPt).length();
				if (dist < near_threshold) {
					foundNearVertex = true;
					break;
				}
			}

			if (foundNearVertex) {
				// 道路セグメントの生成を交差点でストップさせる
				return 2;
			}
		}

		// ターゲットエリア外なら、終了
		if (!targetArea.contains(next_pt)) {
			return 1;
		}

		new_edge->addPoint(next_pt);

		pt = next_pt;
	}

	RoadVertexPtr v = RoadVertexPtr(new RoadVertex(pt));
	tgtDesc = GraphUtil::addVertex(roads, v);
	roads.graph[tgtDesc]->eigenType = type;
	roads.graph[tgtDesc]->new_vertx = true;

	// add edge
	GraphUtil::addEdge(roads, srcDesc, tgtDesc, new_edge);
	
	return 0;
}

void PMRoadGenerator::removeDanglingEdges() {
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
		int count = 0;
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(*vi, roads.graph); ei != eend; ++ei) {
			if (roads.graph[*ei]->valid) count++;
		}

		if (count == 1) {
			roads.graph[*vi]->valid = false;
			RoadOutEdgeIter ei, eend;
			for (boost::tie(ei, eend) = boost::out_edges(*vi, roads.graph); ei != eend; ++ei) {
				roads.graph[*ei]->valid = false;
			}
		}
	}
}

void PMRoadGenerator::saveTensorImage(const cv::Mat& tensor, const std::string& filename) {
	cv::Mat result(tensor.size(), CV_8U, cv::Scalar(255));
	for (int r = 0; r < tensor.rows; r += 50) {
		for (int c = 0; c < tensor.cols; c += 50) {
			int x1 = c;
			int y1 = r;
			float angle = tensor.at<float>(r, c);
			int x2 = x1 + cosf(angle) * 30;
			int y2 = y1 + sinf(angle) * 30;
			cv::line(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0), 1, cv::LINE_AA);
		}
	}

	cv::flip(result, result, 0);
	cv::imwrite(filename.c_str(), result);
}
