classdef Adder < handle
   properties
      cptr
   end
   methods
      function obj = Adder(initval)
         if ~exist('CAdder.oct','file')
           mkoctfile('CAdder.cpp')
         end
         autoload('CAdder_new','CAdder.oct')
         autoload('CAdder_delete','CAdder.oct')
         autoload('CAdder_add','CAdder.oct')
         autoload('CAdder_total','CAdder.oct')
         if nargin<1
           initval=0;
         end
         obj.cptr=CAdder_new(initval);
      end
      function add(obj,n)
         CAdder_add(obj.cptr,n);
      end
      function r = total(obj)
         r=CAdder_total(obj.cptr);
      end
      function delete(obj)
         CAdder_delete(obj.cptr);
      end
   end
end