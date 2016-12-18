<?php
$out = explode("\n",`./puzzle $argv[1] 2>/dev/null`);
$count = array_fill(0,10,0);
foreach($out as $num){
	foreach(str_split($num) as $digit){
		$count[(int)$digit]++;
	}
}
echo implode("\n",$count)."\n\n";
echo count(array_unique($out))."\n";
