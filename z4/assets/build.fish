for name in *.dot;
unflatten -l 3 $name | dot -Tpng -Gdpi=300 -o (echo $name | sed 's/\.[^.]*$//').png;
end;
