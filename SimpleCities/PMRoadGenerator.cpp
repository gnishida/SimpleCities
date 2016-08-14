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
	//srand(12345);

	// make roads along the boundary of the area
	generateRoadsAtBoundary();

	std::list<RoadVertexDesc> seeds;

	// Avenueのシードを生成
	generateAvenueSeeds(seeds);

	// Avenueを生成
	//std::cout << "Avenue generation started." << std::endl;
	{
		int iter;
		for (iter = 0; !seeds.empty() && iter < 200; ) {
			RoadVertexDesc desc = seeds.front();
			seeds.pop_front();

			if (roads.graph[desc]->level < 2) {
				attemptExpansion(RoadEdge::TYPE_AVENUE, desc, seeds);
			}

			iter++;
		}
	}
	//std::cout << "Avenue generation completed." << std::endl;

	seeds.clear();

#if 0

	// Avenueをクリーンナップ
	/*
	if (G::getBool("cleanAvenues")) {
		RoadGeneratorHelper::removeDanglingEdges(roads);
	}
	*/

	// recover the temporarily removed local streets if they are not intersected with other edges
	{
		for (QMap<RoadEdgeDesc, bool>::iterator it = originalLocalStreets.begin(); it != originalLocalStreets.end(); ++it) {
			if (!GraphUtil::isIntersect(roads, roads.graph[it.key()]->polyline)) {
				roads.graph[it.key()]->valid = true;
			}
		}
	}

	// Local streetを生成
	if (true) {
		generateStreetSeeds(seeds);
				
		int iter;
		for (iter = 0; !seeds.empty() && iter < 2000; ) {
			RoadVertexDesc desc = seeds.front();
			seeds.pop_front();

			//std::cout << "attemptExpansion (street): " << iter << " (Seed: " << desc << ")" << std::endl;

			// エリアの外なら、スキップする
			if (!targetArea.contains(roads.graph[desc]->pt)) {
				continue;
			}

			float z = vboRenderManager->getMinTerrainHeight(roads.graph[desc]->pt.x(), roads.graph[desc]->pt.y());
			if (z < G::getFloat("sea_level")) {
				// 水中の頂点は、degree=1のはず！！
				if (GraphUtil::getDegree(roads, desc) > 1) {
					printf("ERROR!!! the vertex %d on the river has degree > 1.\n", desc);
				}
				removeEdge(roads, desc);
				continue;
			}

			attemptExpansion(RoadEdge::TYPE_STREET, desc, seeds);

			if (false) {
				char filename[255];
				sprintf(filename, "road_images/streets_%d.jpg", iter);
				saveRoadImage(roads, seeds, filename);
			}

			iter++;
		}
	}

	// 指定されたエリアでCropping
	/*
	if (G::getBool("cropping")) {
		GraphUtil::extractRoads2(roads, targetArea);
	}
	*/

	/*
	if (G::getBool("cleanStreets")) {
		RoadGeneratorHelper::removeDanglingEdges(roads);
	}
	*/

	//GraphUtil::cleanEdges(roads);
#endif
}

void PMRoadGenerator::generateRoadsAtBoundary() {
	RoadVertexDesc cur_desc;
	for (int i = 0; i < targetArea.size(); ++i) {
		int next = (i + 1) % targetArea.size();

		// create an intersection
		if (i == 0) {
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(targetArea[i]));
			cur_desc = GraphUtil::addVertex(roads, v);
		}

		RoadVertexPtr v = RoadVertexPtr(new RoadVertex(targetArea[next]));
		RoadVertexDesc next_desc = GraphUtil::addVertex(roads, v);

		// add edge
		RoadEdgePtr e = RoadEdgePtr(new RoadEdge(RoadEdge::TYPE_AVENUE, 1));
		e->polyline.push_back(roads.graph[cur_desc]->pt);
		e->polyline.push_back(roads.graph[next_desc]->pt);
		GraphUtil::addEdge(roads, cur_desc, next_desc, e);

		cur_desc = next_desc;
	}
}

/**
 * シード頂点を生成する。
 */
void PMRoadGenerator::generateAvenueSeeds(std::list<RoadVertexDesc>& seeds) {
	seeds.clear();

	BBox bbox = targetArea.envelope();

	RoadVertexPtr v = RoadVertexPtr(new RoadVertex(bbox.midPt()));
	RoadVertexDesc desc = GraphUtil::addVertex(roads, v);
	roads.graph[desc]->level = 0;
	seeds.push_back(desc);
}

/**
 * Local Street用のシードを生成する。
 */
void PMRoadGenerator::generateStreetSeeds(std::list<RoadVertexDesc> &seeds) {
	seeds.clear();

	// 頂点自体を、Local streetsのシードにする
	{
		RoadVertexIter vi, vend;
		for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
			if (!roads.graph[*vi]->valid) continue;

			// The existing roads shoule not be used for the local street genration any more.
			if (roads.graph[*vi]->fixed) continue;

			// エリア外なら、スキップ
			if (!targetArea.contains(roads.graph[*vi]->pt)) continue;

			// 水面下なら、スキップ
			if (vboRenderManager->getMinTerrainHeight(roads.graph[*vi]->pt.x(), roads.graph[*vi]->pt.y()) < G::getFloat("sea_level")) continue;

			if (GraphUtil::getDegree(roads, *vi) == 2) {
				seeds.push_back(*vi);
			}
		}
	}
}

/**
 * このシードを使って、PM方式で道路生成する。
 */
void PMRoadGenerator::attemptExpansion(int roadType, RoadVertexDesc srcDesc, std::list<RoadVertexDesc> &seeds) {
	float roadAngleTolerance = G::getFloat("roadAngleTolerance");

	// 当該頂点から出るエッジの方向を取得する
	float direction = getFirstEdgeAngle(roads, srcDesc);

	for (int i = 0; i < 4; ++i) {
		direction += M_PI * 0.5f;

		if (isRedundantEdge(roads, srcDesc, direction, roadAngleTolerance)) continue;

		// ステップ数、長さ、曲率を決定する
		float length;
		if (roadType == RoadEdge::TYPE_AVENUE) {
			length = G::getFloat("avenueAvgSegmentLength");
		}
		else {
			length = G::getFloat("streetAvgSegmentLength");
		}
		int num_steps = 5;
		float curvature = G::getFloat("road_curvature");

		growRoads(roadType, srcDesc, roads.graph[srcDesc]->level + 1, length, num_steps, direction, curvature, 1, roadAngleTolerance, seeds);
	}
}

void PMRoadGenerator::growRoads(int roadType, RoadVertexDesc srcDesc, int level, float length, int num_steps, float angle, float curvature, int lanes, float angleTolerance, std::list<RoadVertexDesc> &seeds) {
	while (true) {
		RoadVertexDesc tgtDesc;
		if (!growRoadSegment(roadType, srcDesc, level, length, num_steps, angle, curvature, lanes, angleTolerance, tgtDesc, seeds)) break;
		srcDesc = tgtDesc;
	}
}

/**
 * 指定されたpolylineに従って、srcDesc頂点からエッジを伸ばす。
 * エッジの端点が、srcDescとは違うセルに入る場合は、falseを返却する。
 */
bool PMRoadGenerator::growRoadSegment(int roadType, RoadVertexDesc srcDesc, int level, float length, int num_steps, float& angle, float curvature, int lanes, float angleTolerance, RoadVertexDesc& tgtDesc, std::list<RoadVertexDesc> &seeds) {
	float sub_length = length / num_steps;
	int dir = Util::genRand() > 0.5 ? 1 : -1;
	
	QVector2D curPt = roads.graph[srcDesc]->pt;

	RoadEdgePtr new_edge = RoadEdgePtr(new RoadEdge(roadType, lanes));
	new_edge->polyline.push_back(curPt);

	for (int iter = 0; iter < num_steps; ++iter) {
		QVector2D nextPt = curPt + QVector2D(sub_length * cosf(angle), sub_length * sinf(angle));


		bool found = false;
		if (getVertexForSnapping(*vboRenderManager, roads, srcDesc, sub_length * 2.0f, G::getFloat("sea_level"), angle, 0.1f, tgtDesc)) {
			found = true;
		}

		if (found) {
			// エッジを生成
			new_edge->polyline.push_back(roads.graph[tgtDesc]->pt);
			GraphUtil::addEdge(roads, srcDesc, tgtDesc, new_edge);

			return false;
		}

		// もし、新規エッジが、既存グラフと交差するなら、エッジ生成をキャンセル
		QVector2D intPoint;
		RoadEdgeDesc closestEdge;
		if (GraphUtil::isIntersect(roads, curPt, nextPt, closestEdge, intPoint)) {
			new_edge->polyline.push_back(intPoint);

			// 他のエッジにスナップ
			tgtDesc = GraphUtil::splitEdge(roads, closestEdge, intPoint);

			// エッジを生成
			GraphUtil::addEdge(roads, srcDesc, tgtDesc, new_edge);

			// stop the road growth if the next point is outside the target area
			if (!targetArea.contains(nextPt)) return false;

			// continue the road growth from the intersection
			srcDesc = tgtDesc;
			new_edge = RoadEdgePtr(new RoadEdge(roadType, lanes));
			new_edge->polyline.push_back(intPoint);
		}

		/*
		// 遠くにある、近接頂点に向かうよう、方向を変える
		if (getVertexForSnapping(*vboRenderManager, roads, curDesc, sub_length * (num_steps - iter) * 2.0f, G::getFloat("sea_level"), angle, 0.15f, tgtDesc)) {
			QVector2D dir = roads.graph[tgtDesc]->pt - curPt;
			float a = atan2f(dir.y(), dir.x());
			angle += Util::diffAngle(a, angle, false) * 0.5f;
		}
		else {
		*/
			angle = angle + curvature * dir;
		//}

		new_edge->polyline.push_back(nextPt);
		curPt = nextPt;
	}

	RoadVertexPtr v = RoadVertexPtr(new RoadVertex(curPt));
	tgtDesc = GraphUtil::addVertex(roads, v);
	roads.graph[tgtDesc]->level = level;
	seeds.push_back(tgtDesc);

	GraphUtil::addEdge(roads, srcDesc, tgtDesc, new_edge);

	return true;
}

float PMRoadGenerator::getFirstEdgeAngle(RoadGraph& roads, RoadVertexDesc srcDesc) {
	QVector2D dir(1, 0);

	RoadOutEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::out_edges(srcDesc, roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		Polyline2D polyline = GraphUtil::orderPolyLine(roads, *ei, srcDesc);
		dir = polyline[1] - polyline[0];
		break;
	}

	return atan2f(dir.y(), dir.x());
}

/**
* 指定された頂点について、指定されたエッジに似たエッジが既に登録済みかどうかチェックする。
* polylineには、各点の、頂点からのオフセット座標が入る。
* 登録済みのエッジに対しては、エッジの端点への方向ベクトルとpolylineの端点の方向ベクトルのなす角度が30度未満なら、trueを返却する。
*/
bool PMRoadGenerator::isRedundantEdge(RoadGraph& roads, RoadVertexDesc v_desc, const Polyline2D &polyline, float angleTolerance) {
	if (polyline.size() == 0) true;

	RoadOutEdgeIter ei, eend;
	for (boost::tie(ei, eend) = out_edges(v_desc, roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		if (roads.graph[*ei]->polyline.size() == 0) continue;

		RoadVertexDesc tgt = boost::target(*ei, roads.graph);

		if ((roads.graph[*ei]->polyline[0] - roads.graph[v_desc]->pt).lengthSquared() <= (roads.graph[*ei]->polyline.last() - roads.graph[v_desc]->pt).lengthSquared()) {
			if (Util::diffAngle(roads.graph[*ei]->polyline[1] - roads.graph[*ei]->polyline[0], polyline[1]) < angleTolerance) return true;
		}
		else {
			if (Util::diffAngle(roads.graph[*ei]->polyline.nextLast() - roads.graph[*ei]->polyline.last(), polyline[1]) < angleTolerance) return true;
		}

		/*
		if (Util::diffAngle(roads.graph[tgt]->pt - roads.graph[v_desc]->pt, polyline.last()) < angleTolerance) {
		return true;
		}
		*/
	}

	return false;
}

/**
* 指定された頂点について、指定されたエッジに似たエッジが既に登録済みかどうかチェックする。
* polylineには、各点の、頂点からのオフセット座標が入る。
* 登録済みのエッジに対しては、エッジの端点への方向ベクトルとpolylineの端点の方向ベクトルのなす角度が30度未満なら、trueを返却する。
*/
bool PMRoadGenerator::isRedundantEdge(RoadGraph& roads, RoadVertexDesc v_desc, float angle, float angleTolerance) {
	RoadOutEdgeIter ei, eend;
	for (boost::tie(ei, eend) = out_edges(v_desc, roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		if (roads.graph[*ei]->polyline.size() == 0) continue;

		Polyline2D polyline = GraphUtil::orderPolyLine(roads, *ei, v_desc);
		float a = atan2f((polyline[1] - polyline[0]).y(), (polyline[1] - polyline[0]).x());

		if (Util::diffAngle(a, angle) < angleTolerance) return true;
	}

	return false;
}

/**
* snap先の頂点を探す。ただし、deadendの頂点は対象外。また、水面下の頂点も対象外。
* また、方向ベクトルがangle方向からしきい値を超えてる場合、その頂点はスキップする。
* さらに、距離がdistance_threshold未満であること。
*
* @param vboRenderManager		標高を取得するため
* @param roads					道路グラフ
* @param srcDesc				snap元の頂点ID
* @param distance_threshold	snap先との距離のしきい値（これより遠い頂点は、対象外）
* @param z_threshold			snap先までの間の標高のしきい値（これより低い所を通り場合は、対象外）
* @param angle					snap元頂点からの方向基準
* @param angle_threshold		方向のしきい値（方向基準からこのしきい値を超える場合は、対象外）
* @param nearest_desc [OUT]	snap先の頂点ID
* @return						snap先が見つかった場合はtrueを返却する。
*/
bool PMRoadGenerator::getVertexForSnapping(VBORenderManager& vboRenderManager, RoadGraph& roads, RoadVertexDesc srcDesc, float distance_threshold, float z_threshold, float angle, float angle_threshold, RoadVertexDesc& nearest_desc) {
	float distance_threshold2 = distance_threshold * distance_threshold;
	float min_cost = std::numeric_limits<float>::max();
	bool found = false;;

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
		if (!roads.graph[*vi]->valid) continue;
		if (*vi == srcDesc) continue;

		QVector2D vec = roads.graph[*vi]->pt - roads.graph[srcDesc]->pt;
		float angle2 = atan2f(vec.y(), vec.x());
		if (Util::diffAngle(angle, angle2) > angle_threshold) continue;

		float dist = vec.lengthSquared();
		if (dist > distance_threshold2) continue;

		// snap先が水面下かチェック
		float z = vboRenderManager.getMinTerrainHeight(roads.graph[*vi]->pt.x(), roads.graph[*vi]->pt.y());
		if (z < z_threshold) continue;

		// 既存エッジとの交差をチェック
		Polyline2D polyline;
		polyline.push_back(roads.graph[srcDesc]->pt);
		polyline.push_back(roads.graph[*vi]->pt);
		if (GraphUtil::isIntersect(roads, polyline)) continue;

		// 適当なコスト関数で、最適な頂点を探す。
		// 基本的には、距離が近く、角度の差が小さいやつ。でも、係数はむずかしい。。。
		float cost = dist + Util::diffAngle(angle, angle2) * 100.0; // 1000.0

		if (cost < min_cost) {
			min_cost = cost;
			nearest_desc = *vi;
			found = true;
		}
	}

	return found;
}

/**
* Return true if at least one polyline point or vertex is under the sea level.
*/
bool PMRoadGenerator::submerged(int roadType, RoadGraph &roads, VBORenderManager *vboRenderManager) {
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		if (submerged(roadType, roads.graph[*ei]->polyline, vboRenderManager)) return true;
	}

	return false;
}

bool PMRoadGenerator::submerged(int roadType, const Polyline2D &polyline, VBORenderManager *vboRenderManager) {
	for (int i = 0; i < polyline.size(); ++i) {
		float z = vboRenderManager->getTerrainHeight(polyline[i].x(), polyline[i].y());
		if ((roadType == RoadEdge::TYPE_AVENUE && z < G::getFloat("road_min_level")) || (roadType == RoadEdge::TYPE_STREET && z < G::getFloat("road_min_level"))) return true;
	}

	return false;
}

bool PMRoadGenerator::submerged(VBORenderManager* vboRenderManager, Polyline2D& polyline, float seaLevel) {
	Polyline2D finerPolyline = GraphUtil::finerEdge(polyline, 5.0f);
	for (int i = 0; i < finerPolyline.size(); ++i) {
		float z = vboRenderManager->getTerrainHeight(finerPolyline[i].x(), finerPolyline[i].y());
		if (z < seaLevel) return true;
	}

	return false;
}

/**
* 道路を延長し、川を越えさせる。指定された長さ伸ばして、依然として川の中なら、延長をキャンセルする。
*
* @roadType		avenue / local street
* @v_desc			頂点ID
* @seeds			シード
* @angle_threshold この角度の範囲で、最短で渡河できる方向を探す
* @max_length		この距離以上なら、キャンセル
* @return			延長したらtrueを返却する
*/
bool PMRoadGenerator::extendRoadAcrossRiver(RoadGraph& roads, VBORenderManager* vboRenderManager, Polygon2D& targetArea, int roadType, RoadVertexDesc v_desc, std::list<RoadVertexDesc> &seeds, float angle_threshold, float max_length, RoadVertexDesc& tgtDesc) {
	// 既存のエッジから方向を決定する
	float angle;
	int lanes;
	{
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v_desc, roads.graph); ei != eend; ++ei) {
			if (!roads.graph[*ei]->valid) continue;

			lanes = roads.graph[*ei]->lanes;
			Polyline2D polyline = GraphUtil::orderPolyLine(roads, *ei, v_desc);
			QVector2D dir = polyline[0] - polyline[1];
			angle = atan2f(dir.y(), dir.x());
			break;
		}
	}

	QVector2D bestPt;
	bool found = false;
	for (float length = max_length * 0.1f; length <= max_length && !found; length += max_length * 0.1f) {
		for (float th = 0; th <= angle_threshold && !found; th += 0.1f) {
			QVector2D pt = roads.graph[v_desc]->pt + QVector2D(cosf(angle + th), sinf(angle + th)) * length;
			float z = vboRenderManager->getMinTerrainHeight(pt.x(), pt.y());
			if (z >= G::getFloat("sea_level")) {
				bestPt = pt;
				found = true;
			}
		}
		for (float th = 0; th >= -angle_threshold && !found; th -= 0.1f) {
			QVector2D pt = roads.graph[v_desc]->pt + QVector2D(cosf(angle + th), sinf(angle + th)) * length;
			float z = vboRenderManager->getMinTerrainHeight(pt.x(), pt.y());
			if (z >= G::getFloat("sea_level")) {
				bestPt = pt;
				found = true;
			}
		}
	}

	if (!found) return false;

	// エッジ生成
	RoadEdgePtr e = RoadEdgePtr(new RoadEdge(roadType, lanes));
	e->polyline.push_back(roads.graph[v_desc]->pt);
	e->polyline.push_back(bestPt);

	// もし、新規エッジが、既存グラフと交差するなら、エッジ生成をキャンセル
	//RoadVertexDesc tgtDesc;
	QVector2D intPoint;
	RoadEdgeDesc closestEdge;
	if (GraphUtil::isIntersect(roads, e->polyline, v_desc, closestEdge, intPoint)) {
		if (vboRenderManager->getMinTerrainHeight(intPoint.x(), intPoint.y()) < G::getFloat("sea_level")) {
			return false;
		}

		// 60%の確率でキャンセル？
		if (Util::genRand(0, 1) < 0.6f) return false;

		// 交差する箇所で中断させる
		e->polyline[1] = intPoint;

		// 他のエッジにスナップ
		tgtDesc = GraphUtil::splitEdge(roads, closestEdge, intPoint);

		return false;
	}
	else {
		// 頂点を追加
		RoadVertexPtr v = RoadVertexPtr(new RoadVertex(bestPt));
		tgtDesc = GraphUtil::addVertex(roads, v);

		// エリア外なら、onBoundaryフラグをセット
		if (!targetArea.contains(roads.graph[tgtDesc]->pt)) {
			roads.graph[tgtDesc]->onBoundary = true;
		}

		// シードに追加
		// NOTE: エリア外でもとりあえずシードに追加する。
		// 理由: シード頂点へのスナップさせたい時があるので。
		//seeds.push_back(tgtDesc);
	}

	// エッジを追加
	GraphUtil::addEdge(roads, v_desc, tgtDesc, e);

	return true;
}

/**
* 指定された頂点から伸びるエッジを削除する。
* degree=2の頂点については、引き続き、その先のエッジも削除していく。
*
* @param roads			道路グラフ
* @param srcDesc		この頂点から削除を開始する
* @param start_e_desc	このエッジ方向に、削除を開始する
*/
void PMRoadGenerator::removeEdge(RoadGraph& roads, RoadVertexDesc srcDesc, RoadEdgeDesc start_e_desc) {
	QMap<RoadVertexDesc, bool> visited;
	std::list<RoadVertexDesc> queue;

	roads.graph[start_e_desc]->valid = false;
	RoadVertexDesc tgt = boost::target(start_e_desc, roads.graph);
	if (GraphUtil::getDegree(roads, tgt) == 1) {
		queue.push_back(tgt);
	}

	if (GraphUtil::getDegree(roads, srcDesc) == 0) roads.graph[srcDesc]->valid = false;

	while (!queue.empty()) {
		RoadVertexDesc v = queue.front();
		queue.pop_front();

		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v, roads.graph); ei != eend; ++ei) {
			if (!roads.graph[*ei]->valid) continue;

			RoadVertexDesc tgt = boost::target(*ei, roads.graph);

			roads.graph[*ei]->valid = false;
			if (GraphUtil::getDegree(roads, v) == 0) roads.graph[v]->valid = false;
			if (GraphUtil::getDegree(roads, tgt) == 0) roads.graph[tgt]->valid = false;

			if (tgt == srcDesc) continue;
			if (visited[tgt]) continue;

			// 上で既に一本のエッジを無効にしているので、もともとdegree=2の頂点は、残り一本だけエッジが残っているはず。
			// なので、 == 2　ではなく、 == 1　とする。
			if (GraphUtil::getDegree(roads, tgt) == 1) {
				queue.push_back(tgt);
			}
		}
	}
}

/**
* 指定された頂点から伸びるエッジを削除する。
* この頂点から出るエッジは1つであることを前提とする。
*
* @param roads			道路グラフ
* @param srcDesc		この頂点から削除を開始する
*/
void PMRoadGenerator::removeEdge(RoadGraph& roads, RoadVertexDesc srcDesc) {
	QMap<RoadVertexDesc, bool> visited;
	std::list<RoadVertexDesc> queue;

	RoadOutEdgeIter ei, eend;
	boost::tie(ei, eend) = boost::out_edges(srcDesc, roads.graph);
	if (ei == eend) return;

	removeEdge(roads, srcDesc, *ei);
}

/**
* 道路網を画像として保存する。
*/
void PMRoadGenerator::saveRoadImage(RoadGraph& roads, std::list<RoadVertexDesc>& seeds, const char* filename) {
	BBox bbox = GraphUtil::getAABoundingBox(roads, true);
	cv::Mat img(bbox.dy() + 1, bbox.dx() + 1, CV_8UC3, cv::Scalar(0, 0, 0));

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		cv::Scalar color(128, 128, 128);
		if (roads.graph[*ei]->type == RoadEdge::TYPE_STREET) {
			color = cv::Scalar(192, 192, 192);
		}

		for (int pl = 0; pl < roads.graph[*ei]->polyline.size() - 1; ++pl) {
			int x1 = roads.graph[*ei]->polyline[pl].x() - bbox.minPt.x();
			int y1 = img.rows - (roads.graph[*ei]->polyline[pl].y() - bbox.minPt.y());
			int x2 = roads.graph[*ei]->polyline[pl + 1].x() - bbox.minPt.x();
			int y2 = img.rows - (roads.graph[*ei]->polyline[pl + 1].y() - bbox.minPt.y());
			cv::line(img, cv::Point(x1, y1), cv::Point(x2, y2), color, 3);
		}
	}

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
		if (!roads.graph[*vi]->valid) continue;

		int x = roads.graph[*vi]->pt.x() - bbox.minPt.x();
		int y = img.rows - (roads.graph[*vi]->pt.y() - bbox.minPt.y());

		// seedを描画
		if (std::find(seeds.begin(), seeds.end(), *vi) != seeds.end()) {
			cv::circle(img, cv::Point(x, y), 6, cv::Scalar(255, 0, 0), 3);
		}

		// onBoundaryを描画
		if (roads.graph[*vi]->onBoundary) {
			cv::circle(img, cv::Point(x, y), 10, cv::Scalar(0, 255, 255), 3);
		}

		// deadendを描画
		/*
		if (roads.graph[*vi]->deadend) {
			cv::circle(img, cv::Point(x, y), 10, cv::Scalar(0, 0, 255), 3);
		}
		*/

		// 頂点IDを描画
		QString str = QString::number(*vi);// +"/" + roads.graph[*vi]->properties["ex_id"].toString();
		cv::putText(img, str.toUtf8().data(), cv::Point(x, y), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
	}

	cv::imwrite(filename, img);
}