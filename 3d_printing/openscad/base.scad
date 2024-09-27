include <Round-Anything/polyround.scad>

//$fn=120;
RADIO = 1;

ANCHO = 180;
LARGO = 200;
ALTO = 10;
difference(){
    polyRoundExtrude([
          [0, 0, RADIO],
          [ANCHO,0, RADIO],
          [ANCHO, LARGO,RADIO],
          [0, LARGO,RADIO]
        ], length=ALTO, r1=1, r2=9, fn=120);
PARTE_BAJA = 0.95;
translate([ANCHO * (1-PARTE_BAJA)/2,LARGO*(1-PARTE_BAJA)/2,-3])
polyRoundExtrude([
          [0, 0, RADIO],
          [ANCHO *PARTE_BAJA,0, RADIO],
          [ANCHO *PARTE_BAJA, LARGO*PARTE_BAJA,RADIO],
          [0, LARGO*PARTE_BAJA,RADIO]
        ], length=ALTO, r1=10, r2=0, fn=120);
}

translate([7.5,7.5,0.93])
cylinder(8,5,5);


translate([ANCHO-7.5,7.5,0.93])
cylinder(8,5,5);

translate([ANCHO-7.5,LARGO-7.5,0.93])
cylinder(8,5,5);

translate([7.5,LARGO-7.5,0.93])
cylinder(8,5,5);