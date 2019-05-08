#include <octave/oct.h>
#include <octave/uint64NDArray.h>

class Adder
{
  public:
    Adder(double initval) : sum(initval){}
    void add(double a){sum=sum+a;}
    double total(){return sum;}
  private:
    double sum;
};

DEFUN_DLD (CAdder_new, args, nargout, "Adder constructor")
{
  int nargin = args.length ();
  
  double initval(0.0);
  if(nargin>0){
    initval=args(0).matrix_value()(0,0);
  }

  Adder* p_adder=new Adder(initval);
  
  octave_value_list retval(1);
  uint64NDArray ret(1);
  ret(0)=(uint64_t)p_adder;
  retval(0)=ret;

  return retval;
}

DEFUN_DLD (CAdder_delete, args, nargout, "delete")
{
  int nargin = args.length ();

  Adder* p_adder(0);
  if(nargin>0){
    uint64NDArray a=args(0).uint64_array_value();
    uint64_t aa=a(0);
    p_adder=(Adder*)aa;
    delete p_adder;
  }
  
  return octave_value_list ();
}

DEFUN_DLD (CAdder_add, args, nargout, "add number to total")
{
  int nargin = args.length ();

  Adder* p_adder(0);
  if(nargin>0){
    uint64NDArray a=args(0).uint64_array_value();
    uint64_t aa=a(0);
    p_adder=(Adder*)aa;
  }
  
  if(nargin>1){
    p_adder->add(args(1).matrix_value()(0));
  }

  return octave_value_list ();
}

DEFUN_DLD (CAdder_total, args, nargout, "add number to total")
{
  int nargin = args.length ();

  Adder* p_adder(0);
  if(nargin>0){
    uint64NDArray a=args(0).uint64_array_value();
    uint64_t aa=a(0);
    p_adder=(Adder*)aa;
  }
  
  octave_value_list retval(1);
  Matrix ret(1,1);
  if(p_adder!=0)ret(0)=p_adder->total();
  retval(0)=ret;

  return retval;
}
