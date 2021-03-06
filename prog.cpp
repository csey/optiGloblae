#include <iostream>
#include <cfloat> // DBL_MAX
#include <cmath> // std::nextafter
#include <iomanip>  // setprecision
#include "gaol/gaol.h"

// Functions implementations
double dist(interval X,interval Y){
  double inf_XY = abs(X.left()-Y.left());
  double sup_XY = abs(X.right()-Y.right());
  return inf_XY > sup_XY ? inf_XY:sup_XY;
}
float maximum(float a, float b){a > b ? a:b;}
float minimum(float a, float b){a < b ? a:b;}
double machineEpsilon(float EPS){
  // taking a floating type variable
  double prev_epsilon;

  // run until condition satisfy
  while ((1+EPS) != 1){
    // copying value of epsilon into previous epsilon
    prev_epsilon = EPS;
    // dividing epsilon by 2
    EPS /=2;
  }

  // print output of the program
  //std::cout << "Machine Epsilon is : " << prev_epsilon << std::endl;
  return prev_epsilon ;
}
bool typeFormat(double x){bool y; x==floor(x)?y=true:y=false; return y;}
interval doubleTointerval(double x){double epsilon = machineEpsilon(0.5); interval y; typeFormat(x)? y=interval(x): y=interval(x-epsilon,x+epsilon); return y;}
// double comparing
// return true if the difference between a and b is less than absEpsilon, or within relEpsilon percent of the larger of a and b
bool approximatelyEqualAbsRel(double a, double b, double absEpsilon, double relEpsilon){
  // Check if the numbers are really close -- needed when comparing numbers near zero.
  double diff = fabs(a - b);
  //std::cout << "diff = "<< diff << '\n';
  if (diff <= absEpsilon)
    return true;

  // Otherwise fall back to Knuth's algorithm
  return diff <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * relEpsilon);
}
bool greaterEqual(double a, double b){
  double absEpsilon = 1e-7;
  double relEpsilon = 1e-5;
  bool y = approximatelyEqualAbsRel(a,b,absEpsilon,relEpsilon);
  //std::cout << "bool = "<< y << '\n';
  if (!y) {
    y = a > b ;
  }
  return y ;
}
bool lessEqual(double a, double b){
  double absEpsilon = 1e-7;
  double relEpsilon = 1e-5;
  bool y = approximatelyEqualAbsRel(a,b,absEpsilon,relEpsilon);
  if (!y) {
    y = a < b ;
  }
  return y ;
}
class Function{
  public:
    Function() {}
    Function(double a, double b) {}
    virtual interval eval(double x) =0;
    virtual interval eval(interval x) =0;
    virtual interval intervalExtension(interval x) =0;
    virtual interval derivative(interval x) =0;
    virtual ~Function() {}
};
class MyFunction : public Function{
  // (ax + b/x)
private:
  interval a ;
  interval b ;
public:
  MyFunction(){
    std::cout << "Default initialisation" << '\n';
    a = interval(-1) ;
    b = interval(-1);
    std::cout << "### a is initialized to [1,1] ### " << '\n';
    std::cout << "### a is initialized to [1,1] ### " << '\n';
  }
  MyFunction(double a1, double b1) {
    // Two cases : a is integer or not ;
    // Two cases : b is integer or not ;
    if (a1 != 0 && b1 != 0){
      a = doubleTointerval(a1);
      b = doubleTointerval(b1);
      std::cout << "### a set to [" << a.left() << " , "<< a.right() << "]  ###"<< std::endl;
      std::cout << "### b set to [" << b.left() << " , "<< b.right() << "]  ###"<< std::endl;
    }else{
      std::cout << "Error : a and b must be different to 0" << '\n';
      MyFunction();
    }
  }
  interval eval(double y){
    // Precondition : ensure x != 0
    interval x = doubleTointerval(y);
    return eval(x);
  }
  interval eval(interval x){
    return a*x + b/x;
  }
  void set(interval a1,interval b1){
    a = a1;
    b = b1;
  }

  interval intervalExtension(interval x){
    interval y ;
    interval racineGauche = -sqrt(b/a) ;
    interval racineDroit  = sqrt(b/a) ;

    if (x.left() == x.right() && x.right() == 0) {
      y = interval(4,6) & interval(7,9);  //y = interval::emptyset;
    }else if(x.left() < 0 && x.right() >0) {
      y = interval(-GAOL_INFINITY,+GAOL_INFINITY);
    }else{
      if (a.left() > 0 && b.left() > 0) {
        if (greaterEqual(x.left(), racineDroit.right())) {
          y = interval( (eval(x.left())).left(), eval(x.right()).right() );
        }else if(0 < x.left() && lessEqual(x.left(), racineDroit.left())  && greaterEqual(x.right(), racineDroit.right())){
          y = interval( (eval(racineDroit)).left(),maximum( (eval(x.left())).left(), (eval(x.right())).right()));
        }else if(0 < x.left() && lessEqual(x.left(),racineDroit.left()) && 0 < x.right() && lessEqual(x.right(),racineDroit.left())){
          y = interval( (eval(x.right())).right(), (eval(x.left())).left() );
        }else if (lessEqual(x.right(),racineGauche.left()) ){
          y = interval( (eval(x.left())).left(),(eval(x.right())).right());
        }else if(lessEqual(racineGauche.right(),x.right()) && x.right() < 0 && lessEqual(x.left(),racineGauche.left())){
          y = interval(minimum( (eval(x.left())).left(), (eval(x.right())).right() ), (eval(racineGauche)).right() );
        }else if (lessEqual(racineGauche.right(),x.right())  && x.right() < 0 && lessEqual(racineGauche.right(),x.left()) && x.left() < 0){
          y = interval( (eval(x.right())).right(), (eval(x.left())).left());
        }else if(x.left() == 0 && greaterEqual(x.right(),racineDroit.right())){
          y = interval( (eval( racineDroit.left() ) ).left(),+GAOL_INFINITY);
        }else if(x.left() == 0 && x.right() < racineDroit.left() ){
          y = interval( (eval(x.right())).left(),+GAOL_INFINITY);
        }else if(x.right() == 0 && lessEqual(x.left(),racineGauche.left())){
          y = interval(-GAOL_INFINITY, (eval( racineGauche)).right() );
        }else if (x.right() == 0 && x.left() > racineGauche.right() ){
          y = interval(-GAOL_INFINITY, (eval(x.left())).right() );
        }
      }else if(a.left() > 0 && b.right() < 0){
        if (x.right() < 0) {
          y = interval((eval(x.left())).left(),(eval(x.right())).right());
        }else if(x.left() > 0){
          y = interval((eval(x.left())).left(),(eval(x.right())).right());
        }else if (x.left() == 0) {
          y = interval(-GAOL_INFINITY, (eval(x.right())).right());
        }else if(x.right() == 0){
          y = interval((eval(x.left())).left(), +GAOL_INFINITY);
        }
      }else if(a.right() < 0 && b.left() > 0){
        set(-a,-b);
        y = -intervalExtension(x);
        set(-a,-b);
      }else{
        // a < 0 && b < 0
        set(-a,-b);
        y = -intervalExtension(x);
        set(-a,-b);
      }
    }
    return y;
  }
  interval derivative(interval x){
    return a - b/sqr(x);
  }
};

int main(void)
{
  gaol::init();
  // Declaration of parameters a and b
  double a = 1 ;
  double b = 1 ;
  // an instance of MyFunction
  MyFunction f = MyFunction(a,b);
  // Declaration of an example interval X
  interval x (interval(1,5));
  // y is the optimal interval extension of X
  interval y = f.intervalExtension(x);

  // Print the results
  std::cout << "An example :";
  std::cout << "\nLet X = [" << x.left() << " , "<< x.right() << "]"<< std::endl;
  std::cout << "Optimal interval extension : ";
  std::cout << "G(X) = [" << y.left() << " , "<< y.right() << "]"<< std::endl;


  // Simulators realisalisation
  std::cout << "\n******************************\t Begin simulations \t**********************************" << '\n';
  char j = 'o'; // stop critérion
  float inf,sup; // lower and Upper bound
  // Boucle de simulations
  do {

    std::cout << "Give two real a & b not null :" << '\n';
    std::cout << "\tA value for a :" << '\t';
    std::cin >> a;
    std::cout << "\tA value for b :" << '\t';
    std::cin >> b;
    f = MyFunction(a,b);
    std::cout << "Time to give an interval value X :" << '\n';
    std::cout << "\t1. Lower bound\t";
    std::cin >> inf;
    std::cout << "\t2. Upper bound\t";
    std::cin >> sup;
    // assume b >= a
    x = interval(inf,sup);
    y = f.intervalExtension(x);
    std::cout << "\nX = [" << x.left() << " , "<< x.right() << "]"<< std::endl;
    std::cout << "G(X) = [" << y.left() << " , "<< y.right() << "]"<< std::endl;
    std::cout << "---------------------------------------------------------------------------------------" << '\n';
    std::cout << "Do you want to continue simulations ? (n to say no) :\t";
    std::cin >> j;
  } while(j!= 'n');

  gaol::cleanup();
  std::cout << "End of program "<<  std::endl;
}
// End of functions
