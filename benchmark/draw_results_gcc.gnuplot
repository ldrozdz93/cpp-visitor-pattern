### horizontal bar graph
reset session

$Data << EOD
pikus-visit                                 4.35
odwyer-visit                                31.8
vstor-visit                                 11.1
std-variant-without-virtual-calls           1.93
virtual-call-without-visitor                1.46
EOD

set yrange [0:*]      # start at zero, find max from the data
set style fill solid  # solid color boxes
unset key             # turn off all titles

myBoxWidth = 0.8
set offsets 0,0,0.5-myBoxWidth/2.,0.5

set xlabel "call time [ns]"

plot $Data using 2:0:(0):2:($0-myBoxWidth/2.):($0+myBoxWidth/2.):($0+1):ytic(1) with boxxyerror lc var

pause -1 "Hit any key to continue"
### end of code
