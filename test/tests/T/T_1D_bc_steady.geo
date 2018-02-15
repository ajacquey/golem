/* Mesh for test T_1D_bc_steady */

Point(1) = {0, 0, 0, 1.0};
Point(2) = {0, 0, 10, 1.0};
Point(3) = {0, 10, 10, 1.0};
Point(4) = {0, 10, 0, 1.0};
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};
Line Loop(5) = {1, 2, 3, 4};
Plane Surface(6) = {5};
Extrude {40, 0, 0} {
  Surface{6};
}
Extrude {60, 0, 0} {
  Surface{28};
}
Transfinite Line {1, 2, 3, 4, 8, 9, 10, 11, 30, 31, 32, 33} = 2 Using Progression 1;
Transfinite Line {13, 14, 18, 22} = 5 Using Progression 1;
Transfinite Line {35, 36, 40, 44} = 7 Using Progression 1;
Transfinite Surface {6};
Transfinite Surface {15};
Transfinite Surface {19};
Transfinite Surface {23};
Transfinite Surface {27};
Transfinite Surface {28};
Transfinite Surface {37};
Transfinite Surface {41};
Transfinite Surface {45};
Transfinite Surface {49};
Transfinite Surface {50};
Recombine Surface {6, 15, 19, 23, 27, 28, 37, 41, 45, 49, 50};
Transfinite Volume{1} = {1, 2, 3, 4, 5, 6, 10, 14};
Transfinite Volume{2} = {5, 6, 10, 14, 15, 16, 20, 24};
Physical Volume(0) = {1};
Physical Volume(1) = {2};
Physical Surface(0) = {6};
Physical Surface(1) = {50};
Physical Surface(2) = {27, 49};
Physical Surface(3) = {19, 41};
Physical Surface(4) = {15, 37};
Physical Surface(5) = {23, 45};
