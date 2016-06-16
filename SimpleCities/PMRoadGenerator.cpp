#include "PMRoadGenerator.h"
#include <QTest>
#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include "global.h"
#include "Util.h"
#include "GraphUtil.h"
//#include "RoadGeneratorHelper.h"
#include <assert.h>
#include "Utils.h"

void PMRoadGenerator::generateRoadNetwork() {
	srand(12345);

	// mark all the existing vertices as fixed
	{
		RoadVertexIter vi, vend;
		for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
			roads.graph[*vi]->fixed = true;
		}
	}

	// remove all the local streets temporalily
	QMap<RoadEdgeDesc, bool> originalLocalStreets;
	{
		RoadEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
			if (!roads.graph[*ei]->valid) continue;
			if (roads.graph[*ei]->type == RoadEdge::TYPE_STREET) {
				originalLocalStreets[*ei] = true;
				roads.graph[*ei]->valid = false;
			}
		}
	}

	std::list<RoadVertexDesc> seeds;

	// Avenueのシードを生成
	generateAvenueSeeds(seeds);

	// Avenueを生成
	std::cout << "Avenue generation started." << std::endl;
	{
		int iter;
		for (iter = 0; !seeds.empty() && iter < 100; ) {
			RoadVertexDesc desc = seeds.front();
			seeds.pop_front();

			// 水中なら、伸ばして水中から脱出できるなら伸ばす。
			float z = vboRenderManager->getMinTerrainHeight(roads.graph[desc]->pt.x(), roads.graph[desc]->pt.y());
			if (z < G::getFloat("sea_level")) {
				// 水中の頂点は、degree=1のはず！！
				/*assert(GraphUtil::getDegree(roads, desc) == 1);
				if (!extendRoadAcrossRiver(roads, vboRenderManager, targetArea, RoadEdge::TYPE_AVENUE, desc, seeds, 0.3f, G::getFloat("acrossRiverTolerance"))) {
					removeEdge(roads, desc);
				}*/
				continue;
			}

			// エリアの外なら、スキップする
			if (!targetArea.contains(roads.graph[desc]->pt)) {
				continue;
			}

			std::cout << "attemptExpansion (avenue): " << iter << " (Seed: " << desc << ")" << std::endl;
			attemptExpansion(RoadEdge::TYPE_AVENUE, desc, seeds);

			if (false) {
				char filename[255];
				sprintf(filename, "road_images/avenues_%d.jpg", iter);
				saveRoadImage(roads, seeds, filename);
			}

			iter++;
		}
	}
	std::cout << "Avenue generation completed." << std::endl;

	seeds.clear();

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
		for (iter = 0; !seeds.empty() && iter < 1000; ) {
			RoadVertexDesc desc = seeds.front();
			seeds.pop_front();

			std::cout << "attemptExpansion (street): " << iter << " (Seed: " << desc << ")" << std::endl;

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
}

/**
 * シード頂点を生成する。
 */
void PMRoadGenerator::generateAvenueSeeds(std::list<RoadVertexDesc>& seeds) {
	seeds.clear();

	BBox bbox = targetArea.envelope();

	int numSeeds = 1;// (targetArea.area() + 125000) / 250000;
	for (int iter = 0; iter < numSeeds; ++iter) {
		float x, y;
		while (true) {
			x = utils::uniform_rand(bbox.minPt.x(), bbox.maxPt.x());
			y = utils::uniform_rand(bbox.minPt.y(), bbox.maxPt.y());
			bool hoge = targetArea.contains(QVector2D(x, y));
			float z = vboRenderManager->getMinTerrainHeight(x, y);
			if (targetArea.contains(QVector2D(x, y)) && vboRenderManager->getMinTerrainHeight(x, y) > G::getFloat("sea_level")) break;
		}

		addAvenueSeed(QVector2D(x, y), seeds);
	}
}

/**
 * Avenue用のシードを座標ptに追加する。
 *
 * @param pt			シード座標
 * @param seeds			追加されたシードは、seedsに追加される。
 */
void PMRoadGenerator::addAvenueSeed(const QVector2D &pt, std::list<RoadVertexDesc>& seeds) {
	// 頂点を追加し、シードとする
	RoadVertexPtr v = RoadVertexPtr(new RoadVertex(pt));
	RoadVertexDesc desc = GraphUtil::addVertex(roads, v);
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
	// 要注意！！
	// 既に標高はチェック済みなので、このチェックは不要のはず！！！
	float z = vboRenderManager->getMinTerrainHeight(roads.graph[srcDesc]->pt.x(), roads.graph[srcDesc]->pt.y());
	if (z < G::getFloat("sea_level")) {
		printf("ERROR! The current vertex should be above sea leve.\n");
		assert(false);
		return;
	}

	float roadAngleTolerance = G::getFloat("roadAngleTolerance");

	// 当該頂点から出るエッジの方向を取得する
	float direction = getFirstEdgeAngle(roads, srcDesc);

	for (int i = 0; i < 4; ++i) {
		direction += M_PI * 0.5f;

		if (isRedundantEdge(roads, srcDesc, direction, roadAngleTolerance)) continue;

		
		//BBox bbox = targetArea.envelope();
		//float D2 = bbox.area();

		/*std::vector<float> sigma2;
		sigma2.push_back(SQR(G::getDouble("interpolationSigma1")));
		sigma2.push_back(SQR(G::getDouble("interpolationSigma2")));
		for (int i = 2; i < features.size(); ++i) {
			sigma2.push_back(sigma2[0]);
		}*/

		// ステップ数、長さ、曲率を決定する
		float step = 100;
		int num_steps = 5;
		float curvature = 0.2f;
		if (roadType == RoadEdge::TYPE_STREET) {
			step = 20;
		}
			
		// 坂が急なら、キャンセル
		QVector2D pt2 = roads.graph[srcDesc]->pt + QVector2D(cosf(direction), sinf(direction)) * 20.0f;
		float z2 = vboRenderManager->getTerrainHeight(pt2.x(), pt2.y());
		if (z2 - z > tanf(G::getFloat("slopeTolerance")) * 20.0f) return;

		growRoadSegment(roadType, srcDesc, step, num_steps, direction, curvature, 1, roadAngleTolerance, seeds);
	}
}

/**
 * 指定されたpolylineに従って、srcDesc頂点からエッジを伸ばす。
 * エッジの端点が、srcDescとは違うセルに入る場合は、falseを返却する。
 */
bool PMRoadGenerator::growRoadSegment(int roadType, RoadVertexDesc srcDesc, float step, int num_steps, float angle, float curvature, int lanes, float angleTolerance, std::list<RoadVertexDesc> &seeds) {
	const int num_sub_steps = 5;
	float sub_step = step / num_sub_steps;
	
	RoadVertexDesc curDesc = srcDesc;
	QVector2D curPt = roads.graph[srcDesc]->pt;
	RoadVertexDesc tgtDesc;
	bool cancel = false;
	for (int iter = 0; iter < num_steps; ++iter) {
		RoadEdgePtr new_edge = RoadEdgePtr(new RoadEdge(roadType, lanes));
		new_edge->polyline.push_back(roads.graph[curDesc]->pt);

		bool found = false;
		if (getVertexForSnapping(*vboRenderManager, roads, curDesc, step * 2.0f, G::getFloat("sea_level"), angle, 0.3f, tgtDesc)) {
			found = true;
		}

		if (found) {
			// もしスナップ先が、シードじゃないなら、エッジ生成をキャンセル
			/*
			if (std::find(seeds.begin(), seeds.end(), tgtDesc) == seeds.end()) {
				//（要検討。50%の確率ぐらいにすべきか？)
				if (Util::genRand(0, 1) < 0.5f) {
					cancel = true;
					break;
				}
			}
			*/

			// もしスナップ先の頂点が、redundantなエッジを持っているなら、エッジ生成をキャンセル
			Polyline2D snapped_polyline;
			snapped_polyline.push_back(QVector2D(0, 0));
			snapped_polyline.push_back(QVector2D(new_edge->polyline.back() - roads.graph[tgtDesc]->pt));
			if (isRedundantEdge(roads, tgtDesc, snapped_polyline, angleTolerance)) {
				//（とりあえず、ものすごい鋭角の場合は、必ずキャンセル)
				cancel = true;
				break;
			}

			// もし他のエッジに交差するなら、エッジ生成をキャンセル
			// （キャンセルせずに、交差させるべき？）
			QVector2D intPoint;
			RoadEdgeDesc closestEdge;
			new_edge->polyline.push_back(roads.graph[tgtDesc]->pt);
			if (GraphUtil::isIntersect(roads, new_edge->polyline, curDesc, closestEdge, intPoint)) {
				cancel = true;
				break;
			}

			// エッジに沿って、水没チェック
			if (roadType == RoadEdge::TYPE_STREET) {
				if (submerged(vboRenderManager, new_edge->polyline, G::getFloat("sea_level"))) {
					cancel = true;
					break;
				}
			}

			// エッジを生成
			GraphUtil::addEdge(roads, curDesc, tgtDesc, new_edge);

			return true;
		}

		// snap先がなければ、polylineを１歩伸ばす
		for (int sub_iter = 0; sub_iter < num_sub_steps; ++sub_iter) {
			QVector2D pt = new_edge->polyline.back();
			float z = vboRenderManager->getMinTerrainHeight(pt.x(), pt.y());
			QVector2D pt2 = pt + QVector2D(cosf(angle), sinf(angle)) * sub_step;
			float z2 = vboRenderManager->getMinTerrainHeight(pt2.x(), pt2.y());

			if (fabs(z - z2) > 2.5f) {
				curvature = 0.3f;
			}

			{
				float angle1 =  angle + curvature / (float)num_sub_steps;
				QVector2D pt1 = pt + QVector2D(cosf(angle1), sinf(angle1)) * sub_step;
				float z1 = vboRenderManager->getMinTerrainHeight(pt1.x(), pt1.y());

				float angle2 =  angle - curvature / (float)num_sub_steps;
				QVector2D pt2 = pt + QVector2D(cosf(angle2), sinf(angle2)) * sub_step;
				float z2 = vboRenderManager->getMinTerrainHeight(pt2.x(), pt2.y());

				if (fabs(z - z1) < fabs(z - z2)) {
					new_edge->polyline.push_back(pt1);
					angle = angle1;
				} else {
					new_edge->polyline.push_back(pt2);
					angle = angle2;
				}
			}
		}

		// もし、新規エッジが、既存グラフと交差するなら、エッジ生成をキャンセル
		QVector2D intPoint;
		RoadEdgeDesc closestEdge;
		if (GraphUtil::isIntersect(roads, new_edge->polyline, curDesc, closestEdge, intPoint)) {
			// 水面下なら終了
			float z = vboRenderManager->getMinTerrainHeight(intPoint.x(), intPoint.y());
			if (z < G::getFloat("sea_level")) {
				cancel = true;
				break;
			}

			// 60%の確率でキャンセル？
			if (Util::genRand(0, 1) < 0.6f) {
				cancel = true;
				break;
			}

			// 交差する箇所で中断させる
			new_edge->polyline.clear();
			new_edge->polyline.push_back(curPt);
			new_edge->polyline.push_back(intPoint);

			// 他のエッジにスナップ
			tgtDesc = GraphUtil::splitEdge(roads, closestEdge, intPoint);
			roads.graph[tgtDesc]->generationType = "snapped";
			roads.graph[tgtDesc]->properties["group_id"] = roads.graph[closestEdge]->properties["group_id"];
			roads.graph[tgtDesc]->properties.remove("example_desc");

			// エッジを生成
			GraphUtil::addEdge(roads, curDesc, tgtDesc, new_edge);

			return true;
		}

		// streetの場合、水面下ならキャンセル
		if (roadType == RoadEdge::TYPE_STREET) {
			float z = vboRenderManager->getMinTerrainHeight(new_edge->polyline.back().x(), new_edge->polyline.back().y());
			if (z < G::getFloat("sea_level")) {
				cancel = true;
				break;
			}
		}

		if (!found) {
			// 頂点を追加
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(new_edge->polyline.back()));
			v->generationType = "pm";
			v->properties["group_id"] = roads.graph[srcDesc]->properties["group_id"];
			tgtDesc = GraphUtil::addVertex(roads, v);

			// エリア外なら、onBoundaryフラグをセット
			if (!targetArea.contains(roads.graph[tgtDesc]->pt)) {
				roads.graph[tgtDesc]->onBoundary = true;
			}

			// エッジを作成
			GraphUtil::addEdge(roads, curDesc, tgtDesc, new_edge);

			curDesc = tgtDesc;
			curPt = new_edge->polyline.back();
		}

		// エリア外なら、ここでストップ
		if (!targetArea.contains(roads.graph[tgtDesc]->pt)) break;

		// Avenueの場合、水面下ならここでストップ
		if (roadType == RoadEdge::TYPE_AVENUE) {
			float z = vboRenderManager->getMinTerrainHeight(new_edge->polyline.back().x(), new_edge->polyline.back().y());
			if (z < G::getFloat("sea_level")) {
				if (!extendRoadAcrossRiver(roads, vboRenderManager, targetArea, roadType, curDesc, seeds, 0.3f, G::getFloat("acrossRiverTolerance"), tgtDesc)) {
					//removeEdge(roads, curDesc);
					cancel = true;
					break;
				}

				curDesc = tgtDesc;
				curPt = roads.graph[curDesc]->pt;

				//break;
			}
		}

		// 遠くにある、近接頂点に向かうよう、方向を変える
		if (getVertexForSnapping(*vboRenderManager, roads, curDesc, step * (num_steps - iter) * 2.0f, G::getFloat("sea_level"), angle, 0.3f, tgtDesc)) {
			QVector2D dir = roads.graph[tgtDesc]->pt - curPt;
			float a = atan2f(dir.y(), dir.x());
			angle += Util::diffAngle(a, angle, false) * 0.5f;
		}
	}

	if (cancel) {
		while (curDesc != srcDesc) {
			roads.graph[curDesc]->valid = false;
			RoadOutEdgeIter ei, eend;
			boost::tie(ei, eend) = boost::out_edges(curDesc, roads.graph);
			roads.graph[*ei]->valid = false;

			RoadVertexDesc nextDesc = boost::target(*ei, roads.graph);
			curDesc = nextDesc;
		}
		return false;
	} else {
		seeds.push_back(tgtDesc);
		return true;
	}
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

		if (roads.graph[*vi]->deadend) continue;

		QVector2D vec = roads.graph[*vi]->pt - roads.graph[srcDesc]->pt;
		float angle2 = atan2f(vec.y(), vec.x());
		if (Util::diffAngle(angle, angle2) > angle_threshold) continue;

		float dist = vec.lengthSquared();
		if (dist > distance_threshold2) continue;

		// snap先が水面下かチェック
		float z = vboRenderManager.getMinTerrainHeight(roads.graph[*vi]->pt.x(), roads.graph[*vi]->pt.y());
		if (z < z_threshold) continue;

		// 共にexampleの場合、元の座標での相対位置と同じなら、スナップしない
		if (roads.graph[srcDesc]->generationType == "example" && roads.graph[*vi]->generationType == "example") {
			// ToDo
		}

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
		roads.graph[tgtDesc]->generationType = "snapped";
		roads.graph[tgtDesc]->properties["group_id"] = roads.graph[closestEdge]->properties["group_id"];
		roads.graph[tgtDesc]->properties["ex_id"] = roads.graph[closestEdge]->properties["ex_id"];
		roads.graph[tgtDesc]->properties.remove("example_desc");

		return false;
	}
	else {
		// 頂点を追加
		RoadVertexPtr v = RoadVertexPtr(new RoadVertex(bestPt));
		v->generationType = "pm";
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
		if (roads.graph[*vi]->deadend) {
			cv::circle(img, cv::Point(x, y), 10, cv::Scalar(0, 0, 255), 3);
		}

		// 頂点IDを描画
		QString str = QString::number(*vi) + "/" + roads.graph[*vi]->properties["ex_id"].toString();
		cv::putText(img, str.toUtf8().data(), cv::Point(x, y), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
	}

	cv::imwrite(filename, img);
}