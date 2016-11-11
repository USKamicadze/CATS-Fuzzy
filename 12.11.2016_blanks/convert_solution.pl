use strict;
use warnings;

my $file_name = $ARGV[0];
my $text;
{
  local $/ = undef;
  open FILE, $file_name or die "Couldn't open file: $!";
  $text = <FILE>;
  close FILE;
}
#define
$text =~ s/solve/local_solve/g;
$text =~ s/int\s+main\(\s*(void)?\s*\)\s*{/#include "solution.h"\nint solve\(\) {/g;
$text =~ s/(?<!\w)freopen\s*\(\s*"input.txt"\s*,\s*"(.*?)"\s*,\s*stdin\s*\)\s*/_fpin = fopen\("input.txt", "$1"\);cin.open\("input.txt"\);/g;
$text =~ s/(?<!\w)freopen\s*\(\s*"output.txt"\s*,\s*"(.*?)"\s*,\s*stdout\s*\)\s*/_fpout = fopen\("output.txt", "$1"\);cout.open\("output.txt"\);/g;
$text =~ s/(?<!\w)printf\(/fprintf\(_fpout, /g;
$text =~ s/(?<!\w)scanf\(/fscanf\(_fpin, /g;

print $text;