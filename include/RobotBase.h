/**
 *  @file RobotBase.h
 *  @brief ロボットのベース
 *  @author KERI (Github: kerikun11)
 *  @url https://kerikeri.top/
 *  @date 2017.10.30
 */
#pragma once

#include "Agent.h"

namespace MazeLib {

/**
 * @brief 迷路を探索ロボットの基底クラス．継承して仮想関数内を埋めて使用する．
 */
class RobotBase : public Agent {
public:
  RobotBase(Maze &maze) : Agent(maze) {}
  enum Action : char {
    START_STEP,
    START_INIT,
    ST_FULL,
    ST_HALF,
    ST_HALF_STOP,
    TURN_L,
    TURN_R,
    ROTATE_180,
  };
  enum FastAction : char {
    ST_ALONG_FULL = 'S',
    ST_ALONG_HALF = 's',
    ST_DIAG = 'w',
    L_F45 = 'z',
    L_F45P = 'Z',
    R_F45 = 'c',
    R_F45P = 'C',
    L_F90 = 'q',
    R_F90 = 'Q',
    L_FV90 = 'p',
    R_FV90 = 'P',
    L_FS90 = 'L',
    R_FS90 = 'R',
    L_F135 = 'a',
    L_F135P = 'A',
    R_F135 = 'd',
    R_F135P = 'D',
    L_F180 = 'u',
    R_F180 = 'U',
  };
  std::string pathConvertSearchToFast(std::string src, bool diag_enabled) {
    src = (char)ST_ALONG_HALF + src + (char)ST_ALONG_HALF;
    return replaceStringSearchToFast(src, diag_enabled);
  }
  std::string pathConvertSearchToKnown(std::string src) {
    auto f = src.find_first_of(ST_ALONG_FULL);
    auto b = src.find_last_of(ST_ALONG_FULL);
    if (f >= b)
      return src;
    auto fb = src.substr(f, b - f + 1);
    fb = replaceStringSearchToFast(fb, true);
    return src.substr(0, f - 0) + fb + src.substr(b + 1, src.size() - b - 1);
  }
  bool searchRun();
  bool positionIdentifyRun();
  bool endFastRunBackingToStartRun();
  bool fastRun(const bool diagonal);

protected:
  /**
   * @brief 仮想関数．継承して中身を埋める
   */
  virtual void waitForEndAction() {}
  virtual void queueAction(const Action action __attribute__((unused))) {}
  virtual void findWall(bool &left __attribute__((unused)),
                        bool &front __attribute__((unused)),
                        bool &right __attribute__((unused)),
                        bool &back __attribute__((unused))) {}
  virtual void backupMazeToFlash() {}
  virtual void stopDequeue() {}
  virtual void startDequeue() {}
  virtual void calibration() {}
  virtual void calcNextDirsPreCallback() {}
  virtual void calcNextDirsPostCallback(SearchAlgorithm::State prevState
                                        __attribute__((unused)),
                                        SearchAlgorithm::State newState
                                        __attribute__((unused))) {}
  virtual void discrepancyWithKnownWall() {}

private:
  void turnbackSave();
  void queueNextDirs(const Dirs &nextDirs);
  bool generalSearchRun();

  int replace(std::string &src, std::string from, std::string to) const {
    if (from.empty())
      return 0;
    auto pos = src.find(from);
    auto toLen = to.length();
    int i = 0;
    while ((pos = src.find(from, pos)) != std::string::npos) {
      src.replace(pos, from.length(), to);
      pos += toLen;
    }
    return i;
  }
  std::string replaceStringSearchToFast(std::string src, bool diag_enabled) {
    replace(src, "S", "ss");
    replace(src, "L", "ll");
    replace(src, "R", "rr");
    if (diag_enabled) {
      replace(src, "rllllr", "rlplr"); /**< FV90 */
      replace(src, "lrrrrl", "lrPrl"); /**< FV90 */
      replace(src, "sllr", "zlr");     /*< F45 */
      replace(src, "srrl", "crl");     /*< F45 */
      replace(src, "rlls", "rlZ");     /*< F45 P */
      replace(src, "lrrs", "lrC");     /*< F45 P */
      replace(src, "sllllr", "alr");   /*< F135 */
      replace(src, "srrrrl", "drl");   /*< F135 */
      replace(src, "rlllls", "rlA");   /*< F135 P */
      replace(src, "lrrrrs", "lrD");   /*< F135 P */
      replace(src, "slllls", "u");     /*< F180 */
      replace(src, "srrrrs", "U");     /*< F180 */
      replace(src, "rllr", "rlwlr");   /*< ST_DIAG */
      replace(src, "lrrl", "lrwrl");   /*< ST_DIAG */
      replace(src, "slls", "q");       /*< F90 */
      replace(src, "srrs", "Q");       /*< F90 */
      replace(src, "rl", "");
      replace(src, "lr", "");
      replace(src, "ss", "S");
    } else {
      replace(src, "slllls", "u"); /*< F180 */
      replace(src, "srrrrs", "U"); /*< F180 */
      replace(src, "slls", "q");   /*< F90 */
      replace(src, "srrs", "Q");   /*< F90 */
      replace(src, "ll", "L");     /**< FS90 */
      replace(src, "rr", "R");     /**< FS90 */
    }
    return src;
  }
};

} // namespace MazeLib
