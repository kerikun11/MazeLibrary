#include "CLRobotBase.h"

using namespace MazeLib;

class CLRobot : public CLRobotBase {
public:
  CLRobot(const Maze &maze_target) : CLRobotBase(maze_target) {}

protected:
  virtual void queueAction(const Action action) override {
#if 1
    if (getState() == SearchAlgorithm::IDENTIFYING_POSITION &&
        real.first == maze.getStart() && action != ST_HALF_STOP)
      logw << "Visited Start! fake_offset: " << fake_offset << std::endl;
#endif
    CLRobotBase::queueAction(action);
  }
};

int test_measurement() {
  std::ofstream csv("measurement.csv");
  const std::string mazedata_dir = "../mazedata/";
  for (const auto filename : {
           "32MM2018HX.maze",       "32MM2017HX.maze",
           "32MM2016HX.maze",       "32MM2015HX.maze",
           "32MM2014HX.maze",       "32MM2013HX.maze",
           "32MM2012HX.maze",       "16MM2019H_kanazawa.maze",
           "16MM2019H_kansai.maze", "16MM2018C.maze",
           "16MM2018H_semi.maze",   "16MM2017C_East.maze",
           "16MM2017H_Cheese.maze", "16MM2017H_Tashiro.maze",
           "16MM2017CX_pre.maze",   "16MM2017CX.maze",
           "16MM2016C_Chubu.maze",  "16MM2016CX.maze",
           "16MM2013CX.maze",       "08MM2016CF_pre.maze",
       }) {
    std::cout << std::endl;
    std::cout << "Maze File: \t" << filename << std::endl;
    csv << filename;

#if 1
    /* Search Run */
    Maze maze_target = Maze((mazedata_dir + filename).c_str());
    const auto p_robot = std::make_unique<CLRobot>(maze_target);
    CLRobot &robot = *p_robot;
    robot.replaceGoals(maze_target.getGoals());
    const auto t_s = std::chrono::system_clock().now();
    if (!robot.searchRun())
      loge << "Failed to Find a Path to Goal! " << std::endl;
    const auto t_e = std::chrono::system_clock().now();
    const auto us =
        std::chrono::duration_cast<std::chrono::microseconds>(t_e - t_s);
    robot.printResult();
    csv << "," << robot.step << "," << robot.f << "," << robot.l << ","
        << robot.r << "," << robot.b;
    std::cout << "Max Calc Time:\t" << robot.max_usec << "\t[us]" << std::endl;
    // std::cout << "Total Search:\t" << us.count() << "\t[us]" << std::endl;
    csv << "," << robot.max_usec;
    csv << "," << us.count();
    for (const auto diag_enabled : {false, true}) {
      if (!robot.calcShortestDirs(diag_enabled))
        loge << "Failed to Find a Shortest Path! "
             << (diag_enabled ? "diag" : "no_diag") << std::endl;
      robot.fastRun(diag_enabled);
      // robot.printPath();
      robot.endFastRunBackingToStartRun();
      /* Shortest Path Comparison */
      const auto p_at = std::make_unique<Agent>(maze_target);
      Agent &at = *p_at;
      at.calcShortestDirs(diag_enabled);
      if (at.getShortestDirs() != robot.getShortestDirs()) {
        logw << "searched path is not shortest! "
             << (diag_enabled ? "diag" : "no_diag") << std::endl;
        // at.printPath(); robot.printPath();
        logi << "target: "
             << at.getSearchAlgorithm()
                    .getShortestAlgorithm()
                    .getShortestPathCost()
             << " search: "
             << robot.getSearchAlgorithm()
                    .getShortestAlgorithm()
                    .getShortestPathCost()
             << std::endl;
      }
    }
#endif

#if 1
    /* Position Identification Run */
    robot.max_usec = 0;
    StepMap step_map;
    step_map.update(maze_target, maze_target.getGoals(), false, false);
    for (int8_t x = 0; x < MAZE_SIZE; ++x)
      for (int8_t y = 0; y < MAZE_SIZE; ++y)
        for (const auto d : Dir::ENWS()) {
          const auto v = Vector(x, y);
          if (step_map.getStep(v) == STEP_MAX)
            continue; /*< そもそも行けない区画は除外 */
          if (maze_target.isWall(v, d))
            continue; /*< 壁上からは除外 */
          if (v == Vector(0, 0) || v == Vector(0, 1))
            continue;
          /* set fake offset */
          robot.real = robot.fake_offset = VecDir{Vector(x, y), d};
          bool res = robot.positionIdentifyRun();
          if (!res) {
            std::cout << std::endl
                      << "Failed to Identify! fake_offset:\t"
                      << robot.fake_offset << std::endl;
          }
        }
    std::cout << "Max P.I. Time:\t" << robot.max_usec << "\t[us]" << std::endl;
    std::cout << "P.I. wall:\t" << robot.min_id_wall << "\t"
              << robot.max_id_wall << std::endl;
#endif

#if 1
    /* Shortest Algorithm */
    for (const auto diag_enabled : {false, true}) {
      const int n = 100;
      const bool known_only = 0;
      Maze maze = Maze((mazedata_dir + filename).c_str());
      // Maze maze(loadMaze().getGoals());
      const auto p_sa = std::make_unique<ShortestAlgorithm>(maze);
      ShortestAlgorithm &sa = *p_sa;
      Indexes path;
      std::chrono::microseconds sum{0};
      for (int i = 0; i < n; ++i) {
        const auto t_s = std::chrono::system_clock().now();
        sa.calcShortestPath(path, known_only, diag_enabled);
        const auto t_e = std::chrono::system_clock().now();
        const auto us =
            std::chrono::duration_cast<std::chrono::microseconds>(t_e - t_s);
        sum += us;
      }
      std::cout << "Shortest " << (diag_enabled ? "diag" : "no_d") << ":\t"
                << sum.count() / n << "\t[us]" << std::endl;
      std::cout << "PathCost " << (diag_enabled ? "diag" : "no_d") << ":\t"
                << sa.getShortestPathCost() << "\t[ms]" << std::endl;
      // sa.printPath(path);
    }
#endif

#if 0
    /* StepMap */
    for (const auto simple : {true, false}) {
      const bool known_only = 0;
      // Maze maze = Maze(Vectors{Vector(MAZE_SIZE - 1, MAZE_SIZE - 1)});
      Maze &maze = maze_target;
      const auto p = std::make_unique<StepMap>();
      StepMap &map = *p;
      std::chrono::microseconds sum{0};
      const int n = 100;
      for (int i = 0; i < n; ++i) {
        const auto t_s = std::chrono::system_clock().now();
        map.update(maze, maze.getGoals(), known_only, simple);
        const auto t_e = std::chrono::system_clock().now();
        const auto us =
            std::chrono::duration_cast<std::chrono::microseconds>(t_e - t_s);
        sum += us;
      }
      std::cout << "StepMap " << (simple ? "simple" : "normal") << ":\t"
                << sum.count() / n << "\t[us]" << std::endl;
    }
#endif

#if 0
    /* StepMapWall */
    for (const auto simple : {true, false}) {
      const bool known_only = 0;
      // Maze maze = Maze(Vectors{Vector(MAZE_SIZE - 1, MAZE_SIZE - 1)});
      Maze &maze = maze_target;
      WallIndexes dest =
          StepMapWall::convertDestinations(maze, maze.getGoals());
      const auto p = std::make_unique<StepMapWall>();
      StepMapWall &map = *p;
      std::chrono::microseconds sum{0};
      const int n = 100;
      for (int i = 0; i < n; ++i) {
        const auto t_s = std::chrono::system_clock().now();
        map.update(maze, dest, known_only, simple);
        const auto t_e = std::chrono::system_clock().now();
        const auto us =
            std::chrono::duration_cast<std::chrono::microseconds>(t_e - t_s);
        sum += us;
      }
      std::cout << "StepMapWall " << (simple ? "s" : "n") << ":\t"
                << sum.count() / n << "\t[us]" << std::endl;
    }
#endif

#if 0
    /* StepMapSlalom */
    {
      const bool known_only = 0;
      // Maze maze = Maze(Vectors{Vector(MAZE_SIZE - 1, MAZE_SIZE - 1)});
      Maze &maze = maze_target;
      const auto p = std::make_unique<StepMapSlalom>();
      StepMapSlalom &map = *p;
      std::chrono::microseconds sum{0};
      const int n = 100;
      StepMapSlalom::Indexes path;
      for (int i = 0; i < n; ++i) {
        const auto t_s = std::chrono::system_clock().now();
        map.update(maze, StepMapSlalom::EdgeCost(),
                   StepMapSlalom::convertDestinations(maze.getGoals()),
                   known_only);
        map.genPathFromMap(path);
        const auto t_e = std::chrono::system_clock().now();
        const auto us =
            std::chrono::duration_cast<std::chrono::microseconds>(t_e - t_s);
        sum += us;
      }
      std::cout << "StepMapSlalom\t" << sum.count() / n << "\t[us]"
                << std::endl;
      // map.print(maze, path);
    }
#endif

    csv << std::endl;
  }
  std::cout << std::endl << "Measurement End" << std::endl;

  return 0;
}

int main(void) {
  setvbuf(stdout, (char *)NULL, _IONBF, 0);
  return test_measurement();
}
