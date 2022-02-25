/*--------------------------------------------------------------------------*\
 |                                                                          |
 |  Copyright (C) 2022                                                      |
 |                                                                          |
 |         , __                 , __                                        |
 |        /|/  \               /|/  \                                       |
 |         | __/ _   ,_         | __/ _   ,_                                |
 |         |   \|/  /  |  |   | |   \|/  /  |  |   |                        |
 |         |(__/|__/   |_/ \_/|/|(__/|__/   |_/ \_/|/                       |
 |                           /|                   /|                        |
 |                           \|                   \|                        |
 |                                                                          |
 |      Enrico Bertolazzi                                                   |
 |      Dipartimento di Ingegneria Industriale                              |
 |      Universita` degli Studi di Trento                                   |
 |      email: enrico.bertolazzi@unitn.it                                   |
 |                                                                          |
\*--------------------------------------------------------------------------*/

///
/// file: Utils_Poly.hxx
///

#pragma once

#ifndef UTILS_POLY_dot_HH
#define UTILS_POLY_dot_HH

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "Utils.hh"
#include "Eigen/Dense"

namespace Utils {

  template <typename Real>
  class Poly : public Eigen::Matrix<Real,Eigen::Dynamic,1> {
  public:
    typedef int Integer;
    typedef Eigen::Matrix<Real,Eigen::Dynamic,1> dvec_t;
    typedef Poly<Real> Poly_t;

  private:
    Integer m_order;
    dvec_t & to_eigen() { return *static_cast<dvec_t*>(this); }

  public:

    Poly() : m_order(0) {}

    Poly( int order ) : m_order(order) {
      this->resize(order);
      this->setZero();
    }

    Poly( Poly_t const & c )
    { *this = c; }

    Poly( dvec_t const & c ) {
      this->resize( c.size() );
      // per evitare la ricorsione devo chiamare esplicitamente
      // l'operatore = della classe di base.
      this->dvec_t::operator = (c);
      m_order = Integer(c.size());
    }

    // accesso to the Eigen class
    dvec_t const &
    to_eigen() const
    { return * static_cast<dvec_t const *>(this); }

    void
    set_order( Integer order ) {
      m_order = order;
      this->resize( order );
      this->setZero();
    }

    void
    set_degree( Integer degree ) {
      m_order = degree+1;
      this->resize( m_order );
      this->setZero();
    }

    Poly_t & set_scalar( Real a );
    Poly_t & set_monomial( Real a );

    dvec_t  coeffs() const { return to_eigen(); }
    Integer degree() const { return m_order-1; }
    Integer order()  const { return m_order; }

    // stampa il polinomio
    //friend std::basic_ostream & operator<<( std::ostream&, Poly_t const & );

    Real eval( Real x ) const;
    Real eval_D( Real x ) const;
    void eval( Real x, Real & p, Real & Dp ) const;
    Real leading_coeff() const { return this->coeff(m_order-1); }

    void derivative( Poly_t & ) const;
    void integral( Poly_t & ) const;

    //!
    //! Scale polynomial \f$ p(x) = \sum_{i=0}^n a_i x^i \f$ in such a way
    //! \f$ \max_{i=0}^n (|a_i|/S) = 1 \f$. Return the scaling value \f$ S \f$
    //!
    Real normalize( void );

    //!
    //! On the polynomial \f$ p(x) = \sum_{i=0}^n a_i x^i \f$ purge (set to 0)
    //! elements such that \f$ |a_i| \leq \epsilon \f$.
    //!
    void purge( Real epsi );

    //!
    //! Change the polynomial order of the polynomial
    //! \f$ p(x) = \sum_{i=0}^n a_i x^i \f$ in such a way
    //! that \f$ a_n \neq 0 \f$.
    //!
    void adjust_degree( void );

    //!
    //! Count the sign variations of the polynomial
    //! \f$ p(x) = \sum_{i=0}^n a_i x^i \f$ i.e. the number
    //! of sign change of the sequance \f$ [a_0,a_1,\ldots,a_n] \f$.
    //!
    Integer sign_variations( void ) const;

    //!
    //! Change the polynomial in such a way
    //! \f$ p(x) = x^n + \sum_{i=0}^{n-1} a_i x^i \f$.
    //!
    void
    make_monic( void ) {
      this->to_eigen() /= this->coeff(m_order-1);
      this->coeffRef(m_order-1) = 1;
    }

    Poly_t & operator = ( Poly_t const & );

    Poly_t operator-() { return Poly(-this->to_eigen()); }

    Poly_t & operator += ( Poly_t const & ); // somma al polinomio
    Poly_t & operator -= ( Poly_t const & ); // sottrai al polinomio
    Poly_t & operator *= ( Poly_t const & ); // moltiplica il polinomio

    Poly_t & operator += ( Real ); // somma al polinomio
    Poly_t & operator -= ( Real ); // sottrai al polinomio
    Poly_t & operator *= ( Real ); // moltiplica il polinomio

  };

  template <typename Real>
  class Sturm {
  public:
    typedef int        Integer;
    typedef Poly<Real> Poly_t;
    typedef Eigen::Matrix<Real,Eigen::Dynamic,1> dvec_t;

    typedef struct {
      Real    a;
      Real    b;
      Integer va;
      Integer vb;
    } Interval;

  private:

    std::vector<Poly_t>   m_sturm;
    std::vector<Interval> m_intervals;
    Real                  m_a, m_b;
    dvec_t                m_roots;
    Integer               m_max_iter;

  public:

    Sturm() : m_a(0), m_b(0), m_max_iter(20) {}

    //!
    //! Given the polynomial \f$ p(x) \f$ build its Sturm sequence
    //!
    void build( Poly_t const & p );

    //!
    //! Return the length of the stored Sturm sequence.
    //!
    Integer length() const { return Integer(m_sturm.size()); }

    //!
    //! Return the i-th polynomial of the stored Sturm sequence.
    //!
    Poly_t const & get( Integer i ) const { return m_sturm[i]; }

    //!
    //! Conpute the sign variations of the stored Sturm sequence at \f$ x \f$.
    //!
    Integer sign_variations( Real x, bool & on_root ) const;

    //!
    //! Given an interval \f$ [a,b] \f$
    //! compute the subintervals containing a single root.
    //! Return the numbers of intervals (roots) found.
    //!
    Integer separate_roots( Real a, Real b );

    Integer n_roots() const { return Integer(m_intervals.size()); }
    Real a() const { return m_a; }
    Real b() const { return m_b; }
    Interval const & get_interval( Integer i ) const { return m_intervals[i]; }

    //!
    //! compute the roots in the intervals after the separation.
    //!
    void refine_roots( Real tol );

    //!
    //! return a vector with the computed roots
    //!
    dvec_t const & roots() const { return m_roots; }

  };

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */
  template <typename Real>
  Poly<Real> operator + ( Poly<Real> const & a, Poly<Real> const & b );

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */
  template <typename Real>
  Poly<Real> operator + ( Poly<Real> const & a, Real b );

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */
  template <typename Real>
  Poly<Real> operator + ( Real a, Poly<Real> const & b );

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */
  template <typename Real>
  Poly<Real> operator - ( Poly<Real> const & a, Poly<Real> const & b );

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */
  template <typename Real>
  Poly<Real> operator - ( Poly<Real> const & a, Real b );

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */
  template <typename Real>
  Poly<Real> operator - ( Real a, Poly<Real> const & b );

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */
  template <typename Real>
  Poly<Real> operator * ( Poly<Real> const & a, Poly<Real> const & b );

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */
  template <typename Real>
  Poly<Real> operator * ( Real a, Poly<Real> const & b );

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */
  template <typename Real>
  Poly<Real> operator * ( Poly<Real> const & a, Real b );

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */

  template <typename Real>
  void divide(
    Poly<Real> const & p,
    Poly<Real> const & q,
    Poly<Real>       & M,
    Poly<Real>       & R
  );

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */
  template <typename Real>
  void
  GCD(
    Poly<Real> const & p,
    Poly<Real> const & q,
    Poly<Real>       & g
  );

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */
  template <typename Real, typename Char>
  inline
  std::basic_ostream<Char> &
  operator << ( std::basic_ostream<Char> & output, Poly<Real> const & p ) {
    bool    empty = true; // true indica che i coefficienti finora sono nulli
    std::string s = "";   // segno
    Real        c = 0;    // coefficiente
    std::string e = "";   // esponente

    if ( p.order() <= 0 ) { output << "EMPTY!"; return output; }
    if ( p.order() == 1 ) { output << p(0); return output; }
    if ( p.cwiseAbs().maxCoeff() == 0 ) { output << 0; return output; }

    // controlla se esiste il primo coefficiente (grado 0)
    if ( p.coeff(0) != 0 ) {
      output << p.coeff(0);
      empty = false;
    }

    for ( typename Poly<Real>::Integer i=1; i < p.order(); ++i ) {
      // se il coefficiente e` negativo...
      if ( p.coeff(i) < 0 ) {
        // e se i coefficienti precenti erano nulli...
        if ( empty ) {
          s     = ""; // ...non scrive il segno
          c     = p.coeff(i);
          empty = false;
        } else {
          s = " - "; // ...altrimenti scrive il segno come separatore
          c = -p.coeff(i); // e inverte il segno del coefficiente
        }

        // se il coefficiente e` positivo...
      } else if ( p.coeff(i) > 0 ) {
        c = p.coeff(i);
        // e se i coefficienti precenti erano nulli...
        if ( empty ) {
          s     = ""; // ...non scrive il segno
          empty = false;
        } else {
          s = " + "; // ...altrimenti scrive il segno come separatore
        }

        // se il coefficiente e` zero...
      } else {
        continue; // ...procede al prossimo
      }

      // se il grado e` 1 non scrive l'esponente
      if ( i == 1 ) e = "x";
      else          e = fmt::format( "x^{}", i );

      // se il coeff è 1 non lo stampo
      if ( c == 1 ) output << s << e; // stampa
      else          output << s << c << ' ' << e; // stampa
    }
    return output;
  }

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */
  template <typename Real, typename Char>
  inline
  std::basic_ostream<Char> &
  operator << ( std::basic_ostream<Char> & output, Sturm<Real> const & S ) {
    typedef typename Poly<Real>::Integer Integer;
    fmt::print( output, "Sturm sequence\n" );
    for ( Integer i = 0; i < S.length(); ++i )
      fmt::print( output, "P_{}(x) = {}\n", i, S.get(i) );

    Integer n = S.n_roots();
    if ( n > 0 ) {
      fmt::print( output, "roots separation for interval [{},{}]\n", S.a(), S.b() );
      for ( Integer i = 0; i < n; ++i ) {
        typename Sturm<Real>::Interval const & I = S.get_interval( i );
        fmt::print( output, "I = [{}, {}], V = [{}, {}]\n", I.a, I.b, I.va, I.vb );
      }
    }
    return output;
  }

  /*
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  */

  #ifndef UTILS_OS_WINDOWS
  extern template class Poly<float>;
  extern template class Sturm<float>;
  extern template class Poly<double>;
  extern template class Sturm<double>;
  #endif

}

#endif
