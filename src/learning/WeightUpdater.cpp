#include "../../include/learning/WeightUpdater.h"
#include "../../include/learning/LinearSolver.h"
#include "../../include/utils.h"

#include <sstream>

WeightUpdater::WeightUpdater(AlternativesPerformance &ap, Config &conf)
    : ap(ap), conf(conf), solver(ap, conf) {}

WeightUpdater::WeightUpdater(const WeightUpdater &wu)
    : solver(wu.solver), ap(wu.ap), conf(wu.conf) {}

WeightUpdater::~WeightUpdater() {}

void WeightUpdater::updateWeightsAndLambda(MRSortModel &model) {

  if (!this->modelCheck(model)) {
    throw std::invalid_argument("Model's profile doesn't suite the alternative "
                                "performance of this WeightUpdater");
  }

  auto matrix_x = this->computeXMatrix(model);
  auto matrix_y = this->computeYMatrix(model);
  std::pair<float, std::vector<float>> res = solver.solve(matrix_x, matrix_y);

  std::ostringstream ss;
  ss << "Linear problem results - ";

  // update lambda
  model.lambda = res.first;
  ss << "Lambda: " << res.first;
  // update all weights in order
  model.criteria.setWeights(res.second);
  for (int i = 0; i < res.second.size(); i++) {
    ss << " - w" << i << ": " << model.criteria[i].getWeight();
  }
  conf.logger->debug(ss.str());
}

std::vector<std::vector<std::vector<bool>>>
WeightUpdater::computeXMatrix(MRSortModel &model) {
  std::vector<std::vector<std::vector<bool>>> x_matrix;
  auto profs_pt = model.profiles.getPerformanceTable();
  auto alts_pt = ap.getPerformanceTable();
  auto alts_assign = ap.getAlternativesAssignments();

  for (int h = 1; h < profs_pt.size(); h++) {
    std::vector<std::vector<bool>> x_h;
    for (auto alt : alts_pt) {
      std::vector<bool> x_h_alt;
      // if alt is assigned to category h (otherwise, append empty vector)
      if (ap.getAlternativeAssignment(alt[0].name_).rank_ == h) {
        for (int j = 0; j < alt.size(); j++) {
          // condition: aj >= bj_h-1
          x_h_alt.push_back(alt[j].value_ >= profs_pt[h - 1][j].value_);
        }
      }
      x_h.push_back(x_h_alt);
    }
    x_matrix.push_back(x_h);
  }
  return x_matrix;
}

std::vector<std::vector<std::vector<bool>>>
WeightUpdater::computeYMatrix(MRSortModel &model) {

  std::vector<std::vector<std::vector<bool>>> y_matrix;
  auto profs_pt = model.profiles.getPerformanceTable();
  auto alts_pt = ap.getPerformanceTable();
  auto alts_assign = ap.getAlternativesAssignments();
  for (int h = 0; h < profs_pt.size() - 1; h++) {
    std::vector<std::vector<bool>> y_h;
    for (auto alt : alts_pt) {
      std::vector<bool> y_h_alt;
      // if alt is assigned to category h (otherwise, append empty vector)
      if (ap.getAlternativeAssignment(alt[0].name_).rank_ == h) {
        for (int j = 0; j < alt.size(); j++) {
          // condition: aj >= bj_h
          y_h_alt.push_back(alt[j].value_ >= profs_pt[h][j].value_);
        }
      }
      y_h.push_back(y_h_alt);
    }
    y_matrix.push_back(y_h);
  }
  return y_matrix;
}

bool WeightUpdater::modelCheck(MRSortModel &model) {
  if (model.profiles.getNumberCrit() != ap.getNumberCrit()) {
    return false;
  }
  if (model.profiles.getMode() != "alt") {
    throw std::invalid_argument("Model's profile should be in alt mode.");
  }
  if (ap.getMode() != "alt") {
    throw std::invalid_argument(
        "AlternativesPerformance's profile should be in alt mode.");
  }
  // both are supposed to be in mode alt

  std::vector<std::vector<Perf>> profs = model.profiles.getPerformanceTable();
  auto ap_pt = ap.getPerformanceTable();
  for (int i = 0; i < ap.getNumberCrit(); i++) {
    if (profs[0][i].crit_ != ap_pt[0][i].crit_) {
      return false;
    }
  }
  return true;
}
