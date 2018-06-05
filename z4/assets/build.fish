for name in *.dot; 
    unflatten -l 3 $name | dot -Tpng -o $name.png; 
end; 
