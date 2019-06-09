#include "CLRobotBase.h"

using namespace MazeLib;

class CLRobot : public CLRobotBase {
public:
  CLRobot(const Maze &maze_target) : CLRobotBase(maze_target) {}

protected:
  virtual void queueAction(const Action action) override {
#if 0
    if (getState() == SearchAlgorithm::IDENTIFYING_POSITION &&
        real.first == maze.getStart())
      logw << "Visited Start! fake_offset: " << fake_offset << std::endl;
#endif
    CLRobotBase::queueAction(action);
  }
};

int test_measurement() {
  const std::string mazedata_dir = "../mazedata/";
  for (const auto filename : {
           mazedata_dir + "32MM2018HX.maze",
           mazedata_dir + "32MM2017HX.maze",
           mazedata_dir + "32MM2016HX.maze",
           mazedata_dir + "32MM2015HX.maze",
           mazedata_dir + "32MM2014HX.maze",
           mazedata_dir + "32MM2013HX.maze",
           mazedata_dir + "32MM2012HX.maze",
           mazedata_dir + "16MM2018CM.maze",
           mazedata_dir + "16MM2018MS.maze",
           mazedata_dir + "16MM2017CX.maze",
           mazedata_dir + "16MM2017CX_pre.maze",
           mazedata_dir + "16MM2017C_East.maze",
           mazedata_dir + "16MM2017C_Cheese.maze",
           mazedata_dir + "16MM2017Tashiro.maze",
           mazedata_dir + "16MM2016CX.maze",
           mazedata_dir + "16MM2013CX.maze",
       }) {
    std::cout << std::endl;
    std::cout << "Maze File: \t" << filename << std::endl;

#if 1
    /* Search Run */
    Maze maze_target = Maze(filename.c_str());
    const auto p_robot = std::unique_ptr<CLRobot>(new CLRobot(maze_target));
    CLRobot &robot = *p_robot;
    robot.replaceGoals(maze_target.getGoals());
    int sum_total = 0;
    int sum_max = 0;
    const int n = 1;
    for (int i = 0; i < n; ++i) {
      robot.getMaze().reset();
      const auto t_s = std::chrono::system_clock().now();
      if (!robot.searchRun())
        loge << "Failed to Find a Path to Goal! " << std::endl;
      const auto t_e = std::chrono::system_clock().now();
      const auto us =
          std::chrono::duration_cast<std::chrono::microseconds>(t_e - t_s);
      sum_total += us.count();
      sum_max += robot.max_usec;
    }
    robot.printResult();
    std::cout << "Max Calc Time:\t" << sum_max / n << "\t[us]" << std::endl;
    // std::cout << "Total Search:\t" << sum_total / n << "\t[us]" << std::endl;
    for (const auto diag_enabled : {false, true}) {
      if (!robot.calcShortestDirs(diag_enabled))
        loge << "Failed to Find a Shortest Path! "
             << (diag_enabled ? "true" : "false") << std::endl;
      robot.fastRun(diag_enabled);
      // robot.printPath();
      robot.endFastRunBackingToStartRun();
    }
#endif

#if 1
    /* Position Identification Run */
    robot.max_usec = 0;
    StepMap stepMap;
    stepMap.updateSimple(maze_target, maze_target.getGoals(), false);
    for (int8_t x = 0; x < MAZE_SIZE; ++x)
      for (int8_t y = 0; y < MAZE_SIZE; ++y)
        for (const auto d : Dir::ENWS()) {
          const auto v = Vector(x, y);
          if (stepMap.getStep(v) == MAZE_STEP_MAX)
            continue;
          if (v == Vector(0, 0) || v == Vector(0, 1))
            continue;
          // if (maze_target.isWall(v, ed))
          //   continue;
          /* set fake offset */
          robot.real = robot.fake_offset = VecDir{Vector(x, y), d};
          bool res = robot.positionIdentifyRun(Dir::East);
          if (!res) {
            robot.printInfo();
            std::cout << std::endl
                      << "Failed to Identify! fake_offset:\t"
                      << robot.fake_offset << std::endl;
            getc(stdin);
          }
        }
    std::cout << "P.I. Max Time:\t" << robot.max_usec << "\t[us]" << std::endl;
#endif

#if 1
    /* Shortest Algorithm */
    for (const auto diag_enabled : {false, true}) {
      const int n = 100;
      const bool known_only = 0;
      Maze maze(filename.c_str());
      // Maze maze(loadMaze().getGoals());
      const auto p_sa =
          std::unique_ptr<ShortestAlgorithm>(new ShortestAlgorithm(maze));
      ShortestAlgorithm &sa = *p_sa;
      ShortestAlgorithm::Indexes path;
      std::chrono::microseconds sum{0};
      for (int i = 0; i < n; ++i) {
        const auto t_s = std::chrono::system_clock().now();
        sa.calcShortestPath(path, known_only, diag_enabled);
        const auto t_e = std::chrono::system_clock().now();
        const auto us =
            std::chrono::duration_cast<std::chrono::microseconds>(t_e - t_s);
        sum += us;
      }
      std::cout << "Shortest " << (diag_enabled ? "diag" : "along") << ":\t"
                << sum.count() / n << "\t[us]" << std::endl;
      // sa.printPath(std::cout, path);
    }
#endif
  }
  std::cout << std::endl << "Measurement End" << std::endl;

  return 0;
}

int main(void) {
  setvbuf(stdout, (char *)NULL, _IONBF, 0);
  return test_measurement();
}
