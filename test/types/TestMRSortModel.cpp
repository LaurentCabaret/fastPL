#include "../../include/types/Criteria.h"
#include "../../include/types/MRSortModel.h"
#include "../../include/types/PerformanceTable.h"
#include "gtest/gtest.h"
#include <sstream>
#include <utility>

//        crit0  crit1  crit2
// cat0
//         0.8     1     0.4
// cat1
//         0.8    0.1    0.3
// cat2
//         0.6     0     0.2
// base

Criteria getTestCriteria() {
  std::vector<Criterion> crit_vect;
  crit_vect.push_back(Criterion("crit0", -1, 0.5));
  crit_vect.push_back(Criterion("crit1", -1, 0.2));
  crit_vect.push_back(Criterion("crit2", -1, 0.3));
  return Criteria(crit_vect);
}

Profiles getTestProfile() {
  std::vector<Performance> perf_vect;
  Criteria crit = getTestCriteria();
  std::vector<float> given_perf0 = {0.8, 1, 0.4};
  std::vector<float> given_perf1 = {0.8, 0.1, 0.3};
  std::vector<float> given_perf2 = {0.6, 0, 0.2};
  perf_vect.push_back(Performance(crit, given_perf0, "b0"));
  perf_vect.push_back(Performance(crit, given_perf1, "b1"));
  perf_vect.push_back(Performance(crit, given_perf2, "b2"));
  return Profiles(perf_vect);
}

Categories getTestCategories() { return Categories(4); }

TEST(TestMRSortModel, TestBaseConstructorError) {
  Criteria crit = Criteria(1);
  Profiles prof = Profiles(1, crit, "prof");

  Categories cats = Categories(1);
  try {
    MRSortModel mrsort = MRSortModel(crit, prof, cats, 0.5);
    FAIL() << "should have throw invalid argument.";
  } catch (std::invalid_argument const &err) {
    EXPECT_EQ(err.what(),
              std::string("For n categories we must have n - 1 profiles."));
  } catch (...) {
    FAIL() << "should have throw invalid argument.";
  }
}

TEST(TestMRSortModel, TestCategoryAssignments) {
  Profiles profile = getTestProfile();
  Criteria criteria = getTestCriteria();
  Categories categories = getTestCategories();
  MRSortModel mrsort = MRSortModel(criteria, profile, categories, 0.6);

  std::vector<Performance> perf_vect;
  std::vector<float> alt0 = {0.9, 0.6, 0.5};
  std::vector<float> alt1 = {0.9, 0.05, 0.35};
  std::vector<float> alt2 = {0.7, 1, 0.5};
  std::vector<float> alt3 = {0.5, 0, 0.6};
  perf_vect.push_back(Performance(criteria, alt0, "alt0"));
  perf_vect.push_back(Performance(criteria, alt1, "alt1"));
  perf_vect.push_back(Performance(criteria, alt2, "alt2"));
  perf_vect.push_back(Performance(criteria, alt3, "alt3"));
  PerformanceTable pt_ = PerformanceTable(perf_vect);

  std::unordered_map<std::string, Category> expected_assignment;
  expected_assignment["alt0"] = categories.getCategoryOfRank(3);
  expected_assignment["alt1"] = categories.getCategoryOfRank(2);
  expected_assignment["alt2"] = categories.getCategoryOfRank(1);
  expected_assignment["alt3"] = categories.getCategoryOfRank(0);

  // TEST CATEGORY ASSIGNMENT
  std::vector<Perf> a0 = pt_["alt0"];
  std::vector<std::vector<Perf>> profile_pt = profile.getPerformanceTable();
  Category cat = mrsort.categoryAssignment(a0, profile_pt);
  EXPECT_EQ(cat, expected_assignment["alt0"]);

  // TEST CATEGORY ASSIGNMENTS
  AlternativesPerformance ap = mrsort.categoryAssignments(pt_);
  EXPECT_EQ(expected_assignment, ap.getAlternativesAssignments());
}

TEST(TestMRSortModel, TestConcordance) {
  Profiles profile = getTestProfile();
  Criteria criteria = getTestCriteria();
  Categories categories = getTestCategories();
  MRSortModel mrsort = MRSortModel(criteria, profile, categories, 0.6);

  std::vector<Performance> perf_vect;
  std::vector<float> alt0 = {0.9, 0.6, 0.5};
  std::vector<float> alt1 = {0.9, 0.05, 0.35};
  std::vector<float> alt2 = {0.7, 1, 0.5};
  std::vector<float> alt3 = {0.5, 0, 0.6};
  perf_vect.push_back(Performance(criteria, alt0, "alt0"));
  perf_vect.push_back(Performance(criteria, alt1, "alt1"));
  perf_vect.push_back(Performance(criteria, alt2, "alt2"));
  perf_vect.push_back(Performance(criteria, alt3, "alt3"));
  PerformanceTable pt_ = PerformanceTable(perf_vect);

  // TEST COMPUTE CONCORDANCE
  std::vector<Perf> b0 = profile["b0"];
  std::vector<Perf> a0 = pt_["alt0"];
  std::vector<Perf> b2 = profile["b2"];
  std::vector<Perf> a1 = pt_["alt1"];
  float c_alt0_b0 = mrsort.computeConcordance(b0, a0);
  float c_alt1_b2 = mrsort.computeConcordance(b2, a1);
  EXPECT_FLOAT_EQ(c_alt0_b0, 0.8);
  EXPECT_FLOAT_EQ(c_alt1_b2, 1);

  // TEST COMPUTE CONCORDANCE TABLE
  std::unordered_map<std::string, std::unordered_map<std::string, float>> ct =
      mrsort.computeConcordanceTable(pt_);

  EXPECT_FLOAT_EQ(ct["b0"]["alt0"], 0.8);
  EXPECT_FLOAT_EQ(ct["b2"]["alt1"], 1);

  // Display ct
  // for (std::pair<std::string,
  // std::unordered_map<std::string, float>> element
  // :
  //      ct) {
  //   std::cout << "profile " << element.first << std::endl;
  //   for (std::pair<std::string, float> conc : element.second) {
  //     std::cout << conc.first << " conc " << conc.second << std::endl;
  //   }
  // }
}