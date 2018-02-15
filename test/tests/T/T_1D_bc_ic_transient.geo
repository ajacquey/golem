/* Mesh for test T_1D_bc_ic_transient */

Point(1) = {0, 0, 0, 1.0};
Point(2) = {0, 0, 1, 1.0};
Point(3) = {0, 1, 1, 1.0};
Point(4) = {0, 1, 0, 1.0};
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};
Line Loop(5) = {1, 2, 3, 4};
Plane Surface(6) = {5};
Extrude {100, 0, 0} {
  Surface{6};
}
Transfinite Line {1, 2, 3, 4, 8, 9, 10, 11} = 2 Using Progression 1;
Transfinite Line {13, 14, 18, 22} = 101 Using Progression 1;
Transfinite Surface {6};
Transfinite Surface {15};
Transfinite Surface {19};
Transfinite Surface {23};
Transfinite Surface {27};
Transfinite Surface {28};
Recombine Surface {6, 15, 19, 23, 27, 28};
Transfinite Volume{1} = {1, 2, 3, 4, 5, 6, 10, 14};

Point(15) = {0, 2, 0, 1.0};
Point(16) = {0, 2, 1, 1.0};
Point(17) = {0, 3, 1, 1.0};
Point(18) = {0, 3, 0, 1.0};
Line(23) = {15, 16};
Line(24) = {16, 17};
Line(25) = {17, 18};
Line(26) = {18, 15};
Line Loop(27) = {23, 24, 25, 26};
Plane Surface(29) = {27};
Extrude {100, 0, 0} {
  Surface{29};
}
Transfinite Line {23, 24, 25, 26, 31, 32, 33, 34} = 2 Using Progression 1;
Transfinite Line {36, 37, 41, 45} = 101 Using Progression 1;
Transfinite Surface {29};
Transfinite Surface {38};
Transfinite Surface {42};
Transfinite Surface {46};
Transfinite Surface {50};
Transfinite Surface {51};
Recombine Surface {29, 38, 42, 46, 50, 51};
Transfinite Volume{2} = {15, 16, 17, 18, 19, 20, 24, 28};

Physical Volume(0) = {1};
Physical Volume(1) = {2};

Physical Surface(0) = {6};
Physical Surface(1) = {29};
Physical Surface(2) = {28};
Physical Surface(3) = {51};
Physical Surface(4) = {27, 50};
Physical Surface(5) = {19, 42};
Physical Surface(6) = {15, 38};
Physical Surface(7) = {23, 46};
