// IsoVector.h
#if !defined(_ISOVECTOR_H)
#define _ISOVECTOR_H

#include <map>
#include <utility>
#include <math.h>
#include <vector>
#include <string>
#include <libxml/tree.h>

#include "UseMatrixLib.h"

/*!
 we will always need Avogadro's number somewhere
 */
#define AVOGADRO 6.02e23

/*!
 we should define this numerical threshold as a simulation parameter
 its units are kg.
 */
#define EPS_KG 1e-6

/*!
 map isotope (int) to atoms/mass (double)
 */
typedef std::map<int, double> CompMap;

/*! 
 Class Material the object used to transact material objects around the system.
 
 This class keeps track of the isotopic composition of a material using both
 the atomic fractions and the mass fractions, combined with the total number of
 atoms and total mass.
 
 It is an important goal (requirement) that all material objects maintain an
 account of the atoms that is consistent with the account of the mass.
 
 The default mass unit is kilograms.
 */
class IsoVector {

public:
  
  IsoVector(); 

  IsoVector(CompMap initial_comp); 

  /*!
  Used for reading in and initizliaing material recipes.
  */
  IsoVector(xmlNodePtr cur);

  ~IsoVector() {};

  /*!
   loads the recipes from the input file
   */
  static void load_recipes();

  /**
   * get a pointer to the recipe based on its name
   *
   * @param name the name of the recipe for which to return a material pointer.
   */
  static IsoVector recipe(std::string name);                      

  static void printRecipes();

  void print();

  static int recipeCount();                                  

  static std::map<std::string, IsoVector*> recipes_;
  
  /*!
  @brief Adds like isotopes
  */
  IsoVector operator+ (IsoVector rhs_vector);

  /*!
  @brief Subtracts like isotopes

  @exception CycRangeException thrown if subtraction results in a negative
  quantity for any isotope.
  */
  IsoVector operator- (IsoVector rhs_vector);

  /*!
  Compares quantities of like isotopes returning true only if quantity for
  every isotope is equal.
  */
  bool operator== (IsoVector rhs_vector);

  /*!
   Returns the atomic number of the isotope with the given identifier.
   
   @param tope the isotope whose atomic number is being returned
   @return the atomic number
   */
  static int getAtomicNum(int tope);

  /*!
   Returns the mass number of the isotope with the given identifier.
   
   @param tope the isotope whose mass number is being returned
   @return the mass number
   */
  static int getMassNum(int tope);

  /*!
   get material ID
   
   @return ID
   */
  int id() {return ID_;};

  /*!
   returns the total mass of this material object PER UNIT
   */
  double mass();

  /*!
   Returns the current mass of the given isotope, or zero if 
   that isotope isn't present.
   
   @param tope the isotope whose mass in the material will be returned
   @return the mass of the given isotope within the material, or zero
   */
  double mass(int tope);

  /*!
   Sets the total mass of the entire IsoVector maintaining isotopic ratios.
   */
  void setMass(double new_mass);

  /*!
   Sets the mass of the specified isotope.
   */
  void setMass(int tope, double new_mass);

  /*!
   Multiplies the total mass of the entire IsoVector by 'factor' maintaining
   isotopic ratios.
   */
  void multBy(double factor);

  /*!
   returns the total atoms in this material object 
   */
  double atomCount();

  /*!
   Returns the current number density of the given isotope, or zero if 
   that isotope isn't present.
   
   @param tope the isotope whose number density will be returned
   @return the number density of the given isotope, or zero
   */
  double atomCount(int tope);

  /*!
   Sets the total number of atoms for the entire IsoVector maintaining isotopic ratios.
   */
  void setAtomCount(double new_count);

  /*!
   Sets the total number of atoms for the entire IsoVector maintaining isotopic ratios.
   */
  void setAtomCount(int tope, double new_count);

  /*!
   Returns the mass of the given element in this Material.
   
   @param elt the element
   @return the mass of the element (in tons)
   */
  double eltMass(int elt);

  /*!
   Decays this Material object for the given change in time and updates its
   composition map with the new number densities.
   
   @param time_change the number of months to decay
   */
  void executeDecay(double time_change);

  /// Return the atomic (in moles) composition as a std::map<std::string, double>
  CompMap comp();
  
  /*!
   Returns true if the given isotope's number density is less than the 
   conservation of mass tolerance.
   
   @param tope the isotope in question
   @return true iff nd(tope) == 0
   */
  bool isZero(int tope);

  /*!
   Stores the next available material ID
   */
  static int nextID_;

  void validateComposition();

  /*!
   @brief Used to determine validity of isotope defnition.
   @param tope isotope identifier
   @exception thrown if isotope identifier is invalid
   */
  static void validateIsotopeNumber(int tope);

  /*! 
   Unique identifier.
   */
  int ID_;

  /*
   Core isotope composition information stored here.
   */
  CompMap atom_comp_;

};

#endif
