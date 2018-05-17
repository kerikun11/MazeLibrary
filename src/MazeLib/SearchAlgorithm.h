/**
* @file SearchAlgorithm.h
* @brief マイクロマウスの迷路の探索アルゴリズムを扱うクラス
* @author KERI (Github: kerikun11)
* @date 2017.11.05
*/
#pragma once

#include "Maze.h"
#include "StepMap.h"

namespace MazeLib {
	/** @def FIND_ALL_WALL
	*   @brief 全探索するかどうか
	*   true: 全探索
	*   false: 最短になり得ないところは排除
	*/
	#define FIND_ALL_WALL 0
	/** @def SEARCHING_ADDITIALLY_AT_START
	*   @brief 追加探索状態で探索を始める(ゴールを急がない)
	*/
	#define SEARCHING_ADDITIALLY_AT_START 0

	/** @class SearchAlgorithm
	*   @brief 迷路探索アルゴリズムを司るクラス
	*/
	class SearchAlgorithm {
	public:
		/** @brief コンストラクタ
		*   @param maze 使用する迷路の参照
		*   @param goal ゴール区画の配列
		*/
		SearchAlgorithm(Maze& maze, const Vectors& goal) : maze(maze), goal(goal) {}
		/** @enum Status
		*   @brief 進むべき方向の計算結果
		*/
		enum Status{
			Processing,
			Reached,
			Error,
		};
		void replaceGoal(const Vectors& goal) {
			this->goal = goal;
		}
		/** @function isComplete
		*   @brief 最短経路が導出されているか調べる関数
		*/
		bool isComplete(){
			Vectors candidates;
			findShortestCandidates(candidates);
			return candidates.empty();
		}
		enum Status calcNextDirsSearchForGoal(const Vector& cv, const Dir& cd, Dirs& nextDirsKnown, Dirs& nextDirCandidates){
			Vectors candidates;
			for(auto v: goal) if(maze.unknownCount(v)) candidates.push_back(v); //< ゴール区画の未知区画を洗い出す
			if(candidates.empty()) return Reached;
			return calcNextdirsForCandidates(maze, candidates, cv, cd, nextDirsKnown, nextDirCandidates);
		}
		enum Status calcNextDirsSearchAdditionally(const Vector& cv, const Dir& cd, Dirs& nextDirsKnown, Dirs& nextDirCandidates){
			// if(isForceBackToStart) return calcNextDirsBackingToStart(); //< 強制帰還がリクエストされていたら帰る
			Vectors candidates;
			findShortestCandidates(candidates); //< 最短になりうる区画の洗い出し
			if(candidates.empty()) return Reached;
			return calcNextdirsForCandidates(maze, candidates, cv, cd, nextDirsKnown, nextDirCandidates);
		}
		enum Status calcNextDirsBackingToStart(const Vector& cv, const Dir& cd, Dirs& nextDirsKnown, Dirs& nextDirCandidates){
			if(cv == start) return Reached;
			return calcNextdirsForCandidates(maze, {start}, cv, cd, nextDirsKnown, nextDirCandidates);
		}
		enum Status calcNextDirsPositionIdentification(Maze& idMaze, WallLogs& idWallLogs, Vector& cv, const Dir& cd, Dirs& nextDirsKnown, Dirs& nextDirCandidates){
			Vector ans;
			int cnt = countIdentityCandidates(idWallLogs, ans);
			matchCount = cnt;
			if(cnt == 1) {
				cv = cv - idStartVector() + ans;
				return Reached;
			} else if(cnt == 0){
				return Error;
			}
			Vectors candidates;
			for(auto v: {Vector(MAZE_SIZE-1, MAZE_SIZE-1), Vector(MAZE_SIZE-1, 0), Vector(0, MAZE_SIZE-1)}){
				if(idMaze.unknownCount(v)){
					candidates.push_back(v);
					break;
				}
			}
			return calcNextdirsForCandidates(idMaze, candidates, cv, cd, nextDirsKnown, nextDirCandidates);
		}
		enum Status calcNextdirsForCandidates(Maze& maze, const Vectors& dest, const Vector vec, const Dir dir, Dirs& nextDirsKnown, Dirs& nextDirCandidates){
			// stepmap.updatesimple(maze, dest, false);
			// stepmap.calcnextdirs(maze, vec, dir, nextdirsknown, nextdircandidates);
			// if(nextdircandidates.empty()) return error;
			// return processing;

			stepMap.updateSimple(maze, dest, false);
			stepMap.calcNextDirs(maze, vec, dir, nextDirsKnown, nextDirCandidates);
			auto v = vec; for(auto d: nextDirsKnown) v = v.next(d); //< 未知壁区画まで移動する
			Dirs ndcs;
			WallLogs cache;
			while(1){
				if(nextDirCandidates.empty()) break;
				const Dir d = nextDirCandidates[0]; //< 行きたい方向
				ndcs.push_back(d); //< 候補に入れる
				if(maze.isKnown(v, d)) break; //< 既知なら終わり
				cache.push_back(WallLog(v, d, false)); //< 壁をたてるのでキャッシュしておく
				maze.setWall (v, d, true); //< 壁をたてる
				maze.setKnown (v, d, true); //< 既知とする
				Dirs tmp_nds;
				stepMap.updateSimple(maze, dest, false);
				stepMap.calcNextDirs(maze, v, d, tmp_nds, nextDirCandidates);
				if(!tmp_nds.empty()) {
					nextDirCandidates = tmp_nds;
				}
			}
			// キャッシュを復活
			for(auto wl: cache) {
				maze.setWall (Vector(wl), wl.d, false);
				maze.setKnown(Vector(wl), wl.d, false);
			}
			nextDirCandidates = ndcs;
			if(ndcs.empty()) return Error;
			return Processing;
		}
		bool calcShortestDirs(Dirs& shortestDirs, const bool diagonal = true){
			stepMap.update(maze, goal, true, diagonal);
			// stepMap.update(maze, goal, false, diagonal); //< for debug
			shortestDirs.clear();
			auto v = start;
			Dir dir = Dir::North;
			auto prev_dir = dir;
			while(1){
				step_t min_step = MAZE_STEP_MAX;
				const auto& dirs = dir.ordered(prev_dir);
				prev_dir = dir;
				for(const auto& d: dirs){
					if(!maze.canGo(v, d)) continue;
					step_t next_step = stepMap.getStep(v.next(d));
					if(min_step > next_step) {
						min_step = next_step;
						dir = d;
					}
				}
				if(stepMap.getStep(v) <= min_step) return false; //< 失敗
				shortestDirs.push_back(dir);
				v = v.next(dir);
				if(stepMap.getStep(v) == 0) break; //< ゴール区画
			}
			// ゴール区画を行けるところまで直進する
			bool loop = true;
			while(loop){
				loop = false;
				Dirs dirs;
				switch (Dir(dir-prev_dir)) {
					case Dir::Left: dirs = {dir.getRelative(Dir::Right), dir}; break;
					case Dir::Right: dirs = {dir.getRelative(Dir::Left), dir}; break;
					case Dir::Forward: default: dirs = {dir}; break;
				}
				if(!diagonal) dirs = {dir};
				for(const auto& d: dirs){
					if(maze.canGo(v, d)){
						shortestDirs.push_back(d);
						v = v.next(d);
						prev_dir = dir;
						dir = d;
						loop = true;
						break;
					}
				}
			}
			return true;
		}
		const StepMap& getStepMap() const { return stepMap; }
		static const Vector& idStartVector() {
			static auto v = Vector(MAZE_SIZE/2, MAZE_SIZE/2);
			return v;
		}
		int matchCount = 0;

	private:
		Maze& maze; /**< 使用する迷路の参照 */
		StepMap stepMap; /**< 使用するステップマップ */
		const Vector start{0, 0}; /**< スタート区画を定義 */
		Vectors goal; /**< ゴール区画を定義 */

		/** @function findShortestCandidates
		*   @brief ステップマップにより最短経路上になりうる区画を洗い出す
		*/
		bool findShortestCandidates(Vectors& candidates) {
			candidates.clear();
			// 斜めありなしの双方の最短経路上を候補とする
			for(const bool diagonal: {true, false}){
				stepMap.update(maze, goal, false, diagonal);
				auto v = start;
				Dir dir = Dir::North;
				auto prev_dir = dir;
				while(1){
					step_t min_step = MAZE_STEP_MAX;
					const auto& dirs = dir.ordered(prev_dir);
					prev_dir = dir;
					for(const auto& d: dirs){
						if(maze.isWall(v, d)) continue;
						step_t next_step = stepMap.getStep(v.next(d));
						if(min_step > next_step) {
							min_step = next_step;
							dir = d;
						}
					}
					if(stepMap.getStep(v) <= min_step) return false; //< 失敗
					if(maze.unknownCount(v)) candidates.push_back(v);
					v = v.next(dir);
					if(stepMap.getStep(v) == 0) break; //< ゴール区画
				}
				// ゴール区画を行けるところまで直進する
				bool loop = true;
				while(loop){
					loop = false;
					Dirs dirs;
					switch (Dir(dir-prev_dir)) {
						case Dir::Left: dirs = {dir.getRelative(Dir::Right), dir}; break;
						case Dir::Right: dirs = {dir.getRelative(Dir::Left), dir}; break;
						case Dir::Forward: default: dirs = {dir}; break;
					}
					if(!diagonal) dirs = {dir};
					for(const auto& d: dirs){
						if(!maze.isWall(v, d)){
							if(maze.unknownCount(v)) candidates.push_back(v);
							v = v.next(d);
							prev_dir = dir;
							dir = d;
							loop = true;
							break;
						}
					}
				}
			}
			return true; //< 成功
		}
		int countIdentityCandidates(const WallLogs idWallLogs, Vector& ans) const {
			int cnt = 0;
			for(int x=-MAZE_SIZE/2; x<MAZE_SIZE/2; x++)
			for(int y=-MAZE_SIZE/2; y<MAZE_SIZE/2; y++) {
				const Vector offset(x, y);
				int diffs=0;
				int matchs=0;
				int unknown=0;
				for(auto wl: idWallLogs){
					Vector v(wl.x, wl.y);
					Dir d = wl.d;
					if(maze.isKnown(v+offset, d) && maze.isWall(v+offset, d) != wl.b) diffs++;
					if(maze.isKnown(v+offset, d) && maze.isWall(v+offset, d) == wl.b) matchs++;
					if(!maze.isKnown(v+offset, d)) unknown++;
				}
				int size = idWallLogs.size();
				if(diffs <= 4) {
					if(size<4 || unknown<size/2 || matchs>size/2) {
						ans = idStartVector() + offset;
						cnt++;
					}
				}
			}
			return cnt;
		}
	};
}
