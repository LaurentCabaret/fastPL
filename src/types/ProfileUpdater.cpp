#include "../../include/types/ProfileUpdater.h"

#include <cstdlib>
#include <map>
#include <string>

ProfileUpdater::ProfileUpdater(AlternativesPerformance &altPerf_data,
                               float delta) {
  altPerf_data_ = altPerf_data;
  delta_ = delta;
}

ProfileUpdater::ProfileUpdater(const ProfileUpdater &profUp) {
  altPerf_data_ = profUp.altPerf_data_;
  delta_ = profUp.delta_;
}

ProfileUpdater::ProfileUpdater() {}

std::unordered_map<std::string, float> ProfileUpdater::computeAboveDesirability(
    MRSortModel model, std::string critId, Perf perf_prof, Perf perf_prof_above,
    Category cat_below, Category cat_above,
    std::unordered_map<std::string, float> ct_prof) {
  // Data from the problem
  float lambda = model.lambda;
  float weight = model.criteria[critId].getWeight();
  int direction = model.criteria[critId].getDirection();
  float delta = direction * this.delta_;
  std::vector<std::vector<Perf>> altPerfTable_data =
      this.altPerf_data_.getPerformanceTable();
  Profiles profiles_data = model.profiles;

  // Data calculated from the model
  AlternativesPerformance altPerf_model =
      model.categoryAssignments(altPerfTable_data);

  // Alternatives between given profile and above profile
  std::vector<Perf> alt_between = altPerf_model.getAltBetween(
      critId, perf_prof.getValue(), perf_prof_above.getValue());
  // Updating profile value with profile + delta*direction
  Profiles profiles_model_updated = Profiles(profiles_data);

  // Initializing the map of desirability indexes
  std::unordered_map<std::string, float> desirability_above;
  float numerator = 0;
  float denominator = 0;

  for (Perf alt : alt_between) {
    if ((alt.getValue() + delta) * direction > perf_prof_above * direction) {
      std::string altName = alt.getName();
      float c = ct_prof[altName];
      std::string aa_model =
          altPerf_model.getAlternativeAssignment(altName).getCategoryId();
    }
  }
  return desirability_above;
}