use Math::Trig qw(great_circle_distance deg2rad);

while(<>) {
 $_ =~ s/\s$//;
 @fields=split(/ /);

 # Notice the 90 - latitude: phi zero is at the North Pole.
 @L = (deg2rad($fields[4]), deg2rad(90-$fields[3]));
 @T = (deg2rad($fields[6]), deg2rad(90 - $fields[5]));

 $km = great_circle_distance(@L, @T, 6378);

 printf "%s %.1f\n",$_,$km
}
