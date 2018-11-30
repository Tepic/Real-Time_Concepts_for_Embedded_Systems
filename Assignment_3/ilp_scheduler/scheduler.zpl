## Task Set:

# task indices
set Ti := {read "taskset.lst" as "<1n>" comment "#" };

# task release times relative to start of the period
param Tr[Ti] := read "taskset.lst" as "<1n>2n" comment "#";

# task deadlines relative to start of the period
param Td[Ti] := read "taskset.lst" as "<1n>3n" comment "#";

# task execution times
param Te[Ti] := read "taskset.lst" as "<1n>4n" comment "#";

# global period
param p := read "period.dat" as "1n" use 1 comment "#";

## Variables

# start execution at ts
var ts[<i> in Ti] integer >= 0 <= p;

# end execution at te
var te[<i> in Ti] integer >= 0 <= p;

## TODO: add additional variables,
# if necessary

## TODO: add objective
#minimize earlyAsPossible: sum <i> in Ti do ts[i];
maximize noGap:
	sum <i> in Ti do
		sum <j> in Ti with i!=j do
		if te[i] == ts[j] or te[j] == ts[i] 
		then 
		1 
		else 
		0
		end;
		
#minimize noGap:
#	sum <i> in Ti do
#		sum <j> in Ti with i!=j do
#				(vabs(te[j]-ts[i])+(te[j]));
				#vabs(ts[i]-Tr[i]);

## Constraints
## TODO: add constraints
# constraint 4.a)
subto exe_time: forall <i> in Ti do
					ts[i]+Te[i] == te[i];

# constraints 4.b)
subto start_time: forall <i> in Ti do
					ts[i] >= Tr[i];
					
subto deadline_time: forall <i> in Ti do
						te[i]<=Td[i];

subto no_preemption:
	forall <i> in Ti do
		forall <j> in Ti with i!=j do
			vif ts[i]+Te[i]<=ts[j]
			then
				ts[j]>=te[i]
			else
				ts[i]>=te[j]
			end;