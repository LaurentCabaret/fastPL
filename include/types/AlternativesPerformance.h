#ifndef AlternativesPerformance_H
#define AlternativesPerformance_H

/**
 * @file AlternativesPerformance.h
 * @brief Dataset data structure.
 *
 */

#include "Category.h"
#include "Criteria.h"
#include "PerformanceTable.h"
#include <iostream>
#include <iterator>
#include <ostream>
#include <string.h>
#include <unordered_map>
#include <vector>

extern Category default_cat;
extern std::unordered_map<std::string, Category> default_map;

/**
 * @class AlternativesPerformance AlternativesPerformance.h
 * @brief Dataset datastructure
 *
 * The AlternativesPerformance class hold the datastructure that implement a
 * complete dataset. An AlternativesPerformance object hold a PerformanceTable
 * that represents the values of each alternative on each vriterion, and a
 * hashmap that stores the ategory assignment for each alternatives. The
 * hashmap is unordered and link the name of an alternative to the Category
 * object to which it is assigned.
 *
 */
class AlternativesPerformance : public PerformanceTable {
public:
  /**
   * AlternativesPerformance standard constructor (PerformanceTable surcharged)
   *
   * @param perf_vect Vector of performance
   * @param alt_assignment Map of alternative to the assigned category.
   * Default alternatives assigned to empty categories, but good practice would
   * be to create the AlternativePerformance object with an
   * AlternativeAssignment map, otherwise we should use PerformanceTable instead
   */
  AlternativesPerformance(
      std::vector<std::vector<Perf>> &perf_vect,
      std::unordered_map<std::string, Category> &alt_assignment = default_map);

  /**
   * AlternativesPerformance constructor without perf values but set of
   * criteria to evaluate performance over (PerformanceTable surcharged)
   *
   * @param nb_of_perfs Number of performance
   * @param crits Criteria to evaluate performance over
   * @param prefix Prefix to use for the name of each Performance created.
   * Default = "alt"
   * @param alt_assignment Map of alternative to the assigned category.
   * Default alternatives assigned to empty categories, but good practice would
   * be to create the AlternativePerformance object with an
   * AlternativeAssignment map, otherwise we should use PerformanceTable instead
   * */
  AlternativesPerformance(
      int nb_of_perfs, Criteria &crits, std::string prefix = "alt",
      std::unordered_map<std::string, Category> &alt_assignment = default_map);

  /**
   * AlternativesPerformance constructor using an existing performance table
   *
   * @param perf_table Performance table to copy
   * @param alt_assignment Map of alternative assignements to categories
   * Default alternatives assigned to empty categories, but good practice would
   * be to create the AlternativePerformance object with an
   * AlternativeAssignment map, otherwise we should use PerformanceTable instead
   */
  AlternativesPerformance(
      const PerformanceTable &perf_table,
      std::unordered_map<std::string, Category> &alt_assignment = default_map);

  /**
   * AlternativesPerformance constructor by copy
   *
   * @param perfs Based performances to copy
   */
  AlternativesPerformance(const AlternativesPerformance &alt);

  ~AlternativesPerformance();

  friend std::ostream &operator<<(std::ostream &out,
                                  const AlternativesPerformance &alt);

  /**
   * getAlternativesPerformanceMap getter of the alternatives assignments
   *
   * @return alt_assignment_
   */
  std::unordered_map<std::string, Category> getAlternativesAssignments() const;

  // TODO Remove getter and setter
  // Performance wise after some profiling we found that getter and setter can
  // loose a lot of time compared to access directly the variable. Therefore in
  // order to optimize the code, they should be removed.
  /**
   * setAlternativesPerformanceMap getter of the alternatives assignments
   *
   * @param alt_assignment assignment map
   */
  void setAlternativesAssignments(
      std::unordered_map<std::string, Category> &alt_assignment);

  /**
   * getAlternative getter of the assignment of one specified alternative
   *
   * @param altName name of the alternative of which the assignment is requested
   * @return assignment of the alternative
   */
  Category getAlternativeAssignment(std::string altName) const;

  /**
   * setAlternative getter of the assignment of one specified alternative
   *
   * @param altName name of the alternative the category will be assigned to
   * @param cat category to assign
   */
  void setAlternativeAssignment(std::string altName, Category &cat);

  /**
   * getNumberCats compute the number of unique category in the dataset
   *
   * @return n_cats
   */
  int getNumberCats();

  /**
   * getBoundaries return the boundaries of the dataset: the minimal value and
   * the maximal value found
   *
   * @return boudary_pair with first the minimal value, and second the maximal
   * value
   */
  std::pair<float, float> getBoundaries();

private:
  // TODO: Memory could be optimize here, there is no need to store the Category
  // object, we could use a reference instead.

  // Hashmap: key = Alternative Name, value = Category
  std::unordered_map<std::string, Category> alt_assignment_;
};

#endif