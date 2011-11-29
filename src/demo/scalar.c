#include "tbox.h"

#ifdef TB_CONFIG_TYPE_FLOAT
#ifdef TB_CONFIG_TYPE_SCALAR_IS_FIXED
static tb_scalar_t tb_scalar_test_angle[360] = 
{
	0x0
,	0x477
,	0x8ef
,	0xd67
,	0x11df
,	0x1657
,	0x1ace
,	0x1f46
,	0x23be
,	0x2836
,	0x2cae
,	0x3126
,	0x359d
,	0x3a15
,	0x3e8d
,	0x4305
,	0x477d
,	0x4bf4
,	0x506c
,	0x54e4
,	0x595c
,	0x5dd4
,	0x624c
,	0x66c3
,	0x6b3b
,	0x6fb3
,	0x742b
,	0x78a3
,	0x7d1a
,	0x8192
,	0x860a
,	0x8a82
,	0x8efa
,	0x9372
,	0x97e9
,	0x9c61
,	0xa0d9
,	0xa551
,	0xa9c9
,	0xae40
,	0xb2b8
,	0xb730
,	0xbba8
,	0xc020
,	0xc498
,	0xc90f
,	0xcd87
,	0xd1ff
,	0xd677
,	0xdaef
,	0xdf66
,	0xe3de
,	0xe856
,	0xecce
,	0xf146
,	0xf5be
,	0xfa35
,	0xfead
,	0x10325
,	0x1079d
,	0x10c15
,	0x1108c
,	0x11504
,	0x1197c
,	0x11df4
,	0x1226c
,	0x126e4
,	0x12b5b
,	0x12fd3
,	0x1344b
,	0x138c3
,	0x13d3b
,	0x141b2
,	0x1462a
,	0x14aa2
,	0x14f1a
,	0x15392
,	0x1580a
,	0x15c81
,	0x160f9
,	0x16571
,	0x169e9
,	0x16e61
,	0x172d8
,	0x17750
,	0x17bc8
,	0x18040
,	0x184b8
,	0x18930
,	0x18da7
,	0x1921f
,	0x19697
,	0x19b0f
,	0x19f87
,	0x1a3fe
,	0x1a876
,	0x1acee
,	0x1b166
,	0x1b5de
,	0x1ba56
,	0x1becd
,	0x1c345
,	0x1c7bd
,	0x1cc35
,	0x1d0ad
,	0x1d524
,	0x1d99c
,	0x1de14
,	0x1e28c
,	0x1e704
,	0x1eb7c
,	0x1eff3
,	0x1f46b
,	0x1f8e3
,	0x1fd5b
,	0x201d3
,	0x2064b
,	0x20ac2
,	0x20f3a
,	0x213b2
,	0x2182a
,	0x21ca2
,	0x22119
,	0x22591
,	0x22a09
,	0x22e81
,	0x232f9
,	0x23771
,	0x23be8
,	0x24060
,	0x244d8
,	0x24950
,	0x24dc8
,	0x2523f
,	0x256b7
,	0x25b2f
,	0x25fa7
,	0x2641f
,	0x26897
,	0x26d0e
,	0x27186
,	0x275fe
,	0x27a76
,	0x27eee
,	0x28365
,	0x287dd
,	0x28c55
,	0x290cd
,	0x29545
,	0x299bd
,	0x29e34
,	0x2a2ac
,	0x2a724
,	0x2ab9c
,	0x2b014
,	0x2b48b
,	0x2b903
,	0x2bd7b
,	0x2c1f3
,	0x2c66b
,	0x2cae3
,	0x2cf5a
,	0x2d3d2
,	0x2d84a
,	0x2dcc2
,	0x2e13a
,	0x2e5b1
,	0x2ea29
,	0x2eea1
,	0x2f319
,	0x2f791
,	0x2fc09
,	0x30080
,	0x304f8
,	0x30970
,	0x30de8
,	0x31260
,	0x316d7
,	0x31b4f
,	0x31fc7
,	0x3243f
,	0x328b7
,	0x32d2f
,	0x331a6
,	0x3361e
,	0x33a96
,	0x33f0e
,	0x34386
,	0x347fd
,	0x34c75
,	0x350ed
,	0x35565
,	0x359dd
,	0x35e55
,	0x362cc
,	0x36744
,	0x36bbc
,	0x37034
,	0x374ac
,	0x37923
,	0x37d9b
,	0x38213
,	0x3868b
,	0x38b03
,	0x38f7b
,	0x393f2
,	0x3986a
,	0x39ce2
,	0x3a15a
,	0x3a5d2
,	0x3aa49
,	0x3aec1
,	0x3b339
,	0x3b7b1
,	0x3bc29
,	0x3c0a1
,	0x3c518
,	0x3c990
,	0x3ce08
,	0x3d280
,	0x3d6f8
,	0x3db6f
,	0x3dfe7
,	0x3e45f
,	0x3e8d7
,	0x3ed4f
,	0x3f1c7
,	0x3f63e
,	0x3fab6
,	0x3ff2e
,	0x403a6
,	0x4081e
,	0x40c96
,	0x4110d
,	0x41585
,	0x419fd
,	0x41e75
,	0x422ed
,	0x42764
,	0x42bdc
,	0x43054
,	0x434cc
,	0x43944
,	0x43dbc
,	0x44233
,	0x446ab
,	0x44b23
,	0x44f9b
,	0x45413
,	0x4588a
,	0x45d02
,	0x4617a
,	0x465f2
,	0x46a6a
,	0x46ee2
,	0x47359
,	0x477d1
,	0x47c49
,	0x480c1
,	0x48539
,	0x489b0
,	0x48e28
,	0x492a0
,	0x49718
,	0x49b90
,	0x4a008
,	0x4a47f
,	0x4a8f7
,	0x4ad6f
,	0x4b1e7
,	0x4b65f
,	0x4bad6
,	0x4bf4e
,	0x4c3c6
,	0x4c83e
,	0x4ccb6
,	0x4d12e
,	0x4d5a5
,	0x4da1d
,	0x4de95
,	0x4e30d
,	0x4e785
,	0x4ebfc
,	0x4f074
,	0x4f4ec
,	0x4f964
,	0x4fddc
,	0x50254
,	0x506cb
,	0x50b43
,	0x50fbb
,	0x51433
,	0x518ab
,	0x51d22
,	0x5219a
,	0x52612
,	0x52a8a
,	0x52f02
,	0x5337a
,	0x537f1
,	0x53c69
,	0x540e1
,	0x54559
,	0x549d1
,	0x54e48
,	0x552c0
,	0x55738
,	0x55bb0
,	0x56028
,	0x564a0
,	0x56917
,	0x56d8f
,	0x57207
,	0x5767f
,	0x57af7
,	0x57f6e
,	0x583e6
,	0x5885e
,	0x58cd6
,	0x5914e
,	0x595c6
,	0x59a3d
,	0x59eb5
,	0x5a32d
,	0x5a7a5
,	0x5ac1d
,	0x5b094
,	0x5b50c
,	0x5b984
,	0x5bdfc
,	0x5c274
,	0x5c6ec
,	0x5cb63
,	0x5cfdb
,	0x5d453
,	0x5d8cb
,	0x5dd43
,	0x5e1ba
,	0x5e632
,	0x5eaaa
,	0x5ef22
,	0x5f39a
,	0x5f812
,	0x5fc89
,	0x60101
,	0x60579
,	0x609f1
,	0x60e69
,	0x612e1
,	0x61758
,	0x61bd0
,	0x62048
,	0x624c0
,	0x62938
,	0x62daf
,	0x63227
,	0x6369f
,	0x63b17
,	0x63f8f
,	0x64407
};

static tb_scalar_t tb_scalar_test_arc[5][5] = 
{
	{ 0x7fffffff, 	0x0, 		0x0, 		0x0, 		0x0}
, 	{0x7fffffff, 	0x10000, 	0x8000, 	0x5555, 	0x4000}
, 	{0x7fffffff, 	0x20000, 	0x10000, 	0xaaaa, 	0x8000}
, 	{0x7fffffff, 	0x30000, 	0x18000, 	0x10000, 	0xc000}
, 	{0x7fffffff, 	0x40000, 	0x20000, 	0x15555, 	0x10000}

};

static tb_scalar_t tb_scalar_test_atan_a[] = 
{
	0xfff60000
,	0xfff63334
,	0xfff66667
,	0xfff6999a
,	0xfff6cccd
,	0xfff70000
,	0xfff73334
,	0xfff76667
,	0xfff7999a
,	0xfff7cccd
,	0xfff80000
,	0xfff83334
,	0xfff86667
,	0xfff8999a
,	0xfff8cccd
,	0xfff90000
,	0xfff93334
,	0xfff96667
,	0xfff9999a
,	0xfff9cccd
,	0xfffa0000
,	0xfffa3334
,	0xfffa6667
,	0xfffa999a
,	0xfffacccd
,	0xfffb0000
,	0xfffb3334
,	0xfffb6667
,	0xfffb999a
,	0xfffbcccd
,	0xfffc0000
,	0xfffc3334
,	0xfffc6667
,	0xfffc999a
,	0xfffccccd
,	0xfffd0000
,	0xfffd3334
,	0xfffd6667
,	0xfffd999a
,	0xfffdcccd
,	0xfffe0000
,	0xfffe3334
,	0xfffe6667
,	0xfffe999a
,	0xfffecccd
,	0xffff0000
,	0xffff3334
,	0xffff6667
,	0xffff999a
,	0xffffcccd
,	0x0
,	0x3333
,	0x6666
,	0x9999
,	0xcccc
,	0x10000
,	0x13333
,	0x16666
,	0x19999
,	0x1cccc
,	0x20000
,	0x23333
,	0x26666
,	0x29999
,	0x2cccc
,	0x30000
,	0x33333
,	0x36666
,	0x39999
,	0x3cccc
,	0x40000
,	0x43333
,	0x46666
,	0x49999
,	0x4cccc
,	0x50000
,	0x53333
,	0x56666
,	0x59999
,	0x5cccc
,	0x60000
,	0x63333
,	0x66666
,	0x69999
,	0x6cccc
,	0x70000
,	0x73333
,	0x76666
,	0x79999
,	0x7cccc
,	0x80000
,	0x83333
,	0x86666
,	0x89999
,	0x8cccc
,	0x90000
,	0x93333
,	0x96666
,	0x99999
,	0x9cccc
,	0xa0000

};

#else
static tb_scalar_t tb_scalar_test_angle[360] = 
{
	0.000000
,	0.017453
,	0.034907
,	0.052360
,	0.069813
,	0.087266
,	0.104720
,	0.122173
,	0.139626
,	0.157080
,	0.174533
,	0.191986
,	0.209440
,	0.226893
,	0.244346
,	0.261799
,	0.279253
,	0.296706
,	0.314159
,	0.331613
,	0.349066
,	0.366519
,	0.383972
,	0.401426
,	0.418879
,	0.436332
,	0.453786
,	0.471239
,	0.488692
,	0.506145
,	0.523599
,	0.541052
,	0.558505
,	0.575959
,	0.593412
,	0.610865
,	0.628319
,	0.645772
,	0.663225
,	0.680678
,	0.698132
,	0.715585
,	0.733038
,	0.750492
,	0.767945
,	0.785398
,	0.802851
,	0.820305
,	0.837758
,	0.855211
,	0.872665
,	0.890118
,	0.907571
,	0.925025
,	0.942478
,	0.959931
,	0.977384
,	0.994838
,	1.012291
,	1.029744
,	1.047198
,	1.064651
,	1.082104
,	1.099557
,	1.117011
,	1.134464
,	1.151917
,	1.169371
,	1.186824
,	1.204277
,	1.221730
,	1.239184
,	1.256637
,	1.274090
,	1.291544
,	1.308997
,	1.326450
,	1.343904
,	1.361357
,	1.378810
,	1.396263
,	1.413717
,	1.431170
,	1.448623
,	1.466077
,	1.483530
,	1.500983
,	1.518436
,	1.535890
,	1.553343
,	1.570796
,	1.588250
,	1.605703
,	1.623156
,	1.640609
,	1.658063
,	1.675516
,	1.692969
,	1.710423
,	1.727876
,	1.745329
,	1.762783
,	1.780236
,	1.797689
,	1.815142
,	1.832596
,	1.850049
,	1.867502
,	1.884956
,	1.902409
,	1.919862
,	1.937315
,	1.954769
,	1.972222
,	1.989675
,	2.007129
,	2.024582
,	2.042035
,	2.059489
,	2.076942
,	2.094395
,	2.111848
,	2.129302
,	2.146755
,	2.164208
,	2.181662
,	2.199115
,	2.216568
,	2.234021
,	2.251475
,	2.268928
,	2.286381
,	2.303835
,	2.321288
,	2.338741
,	2.356194
,	2.373648
,	2.391101
,	2.408554
,	2.426008
,	2.443461
,	2.460914
,	2.478368
,	2.495821
,	2.513274
,	2.530727
,	2.548181
,	2.565634
,	2.583087
,	2.600541
,	2.617994
,	2.635447
,	2.652900
,	2.670354
,	2.687807
,	2.705260
,	2.722714
,	2.740167
,	2.757620
,	2.775074
,	2.792527
,	2.809980
,	2.827433
,	2.844887
,	2.862340
,	2.879793
,	2.897247
,	2.914700
,	2.932153
,	2.949606
,	2.967060
,	2.984513
,	3.001966
,	3.019420
,	3.036873
,	3.054326
,	3.071779
,	3.089233
,	3.106686
,	3.124139
,	3.141593
,	3.159046
,	3.176499
,	3.193953
,	3.211406
,	3.228859
,	3.246312
,	3.263766
,	3.281219
,	3.298672
,	3.316126
,	3.333579
,	3.351032
,	3.368485
,	3.385939
,	3.403392
,	3.420845
,	3.438299
,	3.455752
,	3.473205
,	3.490659
,	3.508112
,	3.525565
,	3.543018
,	3.560472
,	3.577925
,	3.595378
,	3.612832
,	3.630285
,	3.647738
,	3.665191
,	3.682645
,	3.700098
,	3.717551
,	3.735005
,	3.752458
,	3.769911
,	3.787364
,	3.804818
,	3.822271
,	3.839724
,	3.857178
,	3.874631
,	3.892084
,	3.909538
,	3.926991
,	3.944444
,	3.961897
,	3.979351
,	3.996804
,	4.014257
,	4.031711
,	4.049164
,	4.066617
,	4.084070
,	4.101524
,	4.118977
,	4.136430
,	4.153884
,	4.171337
,	4.188790
,	4.206243
,	4.223697
,	4.241150
,	4.258603
,	4.276057
,	4.293510
,	4.310963
,	4.328417
,	4.345870
,	4.363323
,	4.380776
,	4.398230
,	4.415683
,	4.433136
,	4.450590
,	4.468043
,	4.485496
,	4.502949
,	4.520403
,	4.537856
,	4.555309
,	4.572763
,	4.590216
,	4.607669
,	4.625123
,	4.642576
,	4.660029
,	4.677482
,	4.694936
,	4.712389
,	4.729842
,	4.747296
,	4.764749
,	4.782202
,	4.799655
,	4.817109
,	4.834562
,	4.852015
,	4.869469
,	4.886922
,	4.904375
,	4.921828
,	4.939282
,	4.956735
,	4.974188
,	4.991642
,	5.009095
,	5.026548
,	5.044002
,	5.061455
,	5.078908
,	5.096361
,	5.113815
,	5.131268
,	5.148721
,	5.166175
,	5.183628
,	5.201081
,	5.218534
,	5.235988
,	5.253441
,	5.270894
,	5.288348
,	5.305801
,	5.323254
,	5.340708
,	5.358161
,	5.375614
,	5.393067
,	5.410521
,	5.427974
,	5.445427
,	5.462881
,	5.480334
,	5.497787
,	5.515240
,	5.532694
,	5.550147
,	5.567600
,	5.585054
,	5.602507
,	5.619960
,	5.637413
,	5.654867
,	5.672320
,	5.689773
,	5.707227
,	5.724680
,	5.742133
,	5.759587
,	5.777040
,	5.794493
,	5.811946
,	5.829400
,	5.846853
,	5.864306
,	5.881760
,	5.899213
,	5.916666
,	5.934119
,	5.951573
,	5.969026
,	5.986479
,	6.003933
,	6.021386
,	6.038839
,	6.056293
,	6.073746
,	6.091199
,	6.108652
,	6.126106
,	6.143559
,	6.161012
,	6.178466
,	6.195919
,	6.213372
,	6.230825
,	6.248279
,	6.265732

};

static tb_scalar_t tb_scalar_test_arc[5][5] = 
{
	{TB_FLOAT_INF, 0.000000, 0.000000, 0.000000, 0.000000}
, 	{TB_FLOAT_INF, 1.000000, 0.500000, 0.333333, 0.250000}
, 	{TB_FLOAT_INF, 2.000000, 1.000000, 0.666667, 0.500000}
, 	{TB_FLOAT_INF, 3.000000, 1.500000, 1.000000, 0.750000}
, 	{TB_FLOAT_INF, 4.000000, 2.000000, 1.333333, 1.000000}

};

static tb_scalar_t tb_scalar_test_atan_a[] = 
{
	-10.000000
,	-9.800000
,	-9.600000
,	-9.400000
,	-9.200000
,	-9.000000
,	-8.800000
,	-8.600000
,	-8.400000
,	-8.200000
,	-8.000000
,	-7.800000
,	-7.600000
,	-7.400000
,	-7.200000
,	-7.000000
,	-6.800000
,	-6.600000
,	-6.400000
,	-6.200000
,	-6.000000
,	-5.800000
,	-5.600000
,	-5.400000
,	-5.200000
,	-5.000000
,	-4.800000
,	-4.600000
,	-4.400000
,	-4.200000
,	-4.000000
,	-3.800000
,	-3.600000
,	-3.400000
,	-3.200000
,	-3.000000
,	-2.800000
,	-2.600000
,	-2.400000
,	-2.200000
,	-2.000000
,	-1.800000
,	-1.600000
,	-1.400000
,	-1.200000
,	-1.000000
,	-0.800000
,	-0.600000
,	-0.400000
,	-0.200000
,	0.000000
,	0.200000
,	0.400000
,	0.600000
,	0.800000
,	1.000000
,	1.200000
,	1.400000
,	1.600000
,	1.800000
,	2.000000
,	2.200000
,	2.400000
,	2.600000
,	2.800000
,	3.000000
,	3.200000
,	3.400000
,	3.600000
,	3.800000
,	4.000000
,	4.200000
,	4.400000
,	4.600000
,	4.800000
,	5.000000
,	5.200000
,	5.400000
,	5.600000
,	5.800000
,	6.000000
,	6.200000
,	6.400000
,	6.600000
,	6.800000
,	7.000000
,	7.200000
,	7.400000
,	7.600000
,	7.800000
,	8.000000
,	8.200000
,	8.400000
,	8.600000
,	8.800000
,	9.000000
,	9.200000
,	9.400000
,	9.600000
,	9.800000
,	10.000000
};

#endif

static tb_void_t tb_scalar_test_constant()
{
	tb_printf("[scalar]: one = %f\n", 	tb_scalar_to_float(TB_SCALAR_ONE));
	tb_printf("[scalar]: half = %f\n", 	tb_scalar_to_float(TB_SCALAR_HALF));
	tb_printf("[scalar]: max = %f\n", 	tb_scalar_to_float(TB_SCALAR_MAX));
	tb_printf("[scalar]: min = %f\n", 	tb_scalar_to_float(TB_SCALAR_MIN));
	tb_printf("[scalar]: nan = %f\n", 	tb_scalar_to_float(TB_SCALAR_NAN));
	tb_printf("[scalar]: inf = %f\n", 	tb_scalar_to_float(TB_SCALAR_INF));
	tb_printf("[scalar]: pi = %f\n", 	tb_scalar_to_float(TB_SCALAR_PI));
	tb_printf("[scalar]: sqrt2 = %f\n", tb_scalar_to_float(TB_SCALAR_SQRT2));
}
static tb_void_t tb_scalar_test_round(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_int_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_round(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: round(%f): %d, %lld ms\n", tb_scalar_to_float(x), r, t);
}
static tb_void_t tb_scalar_test_floor(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_int_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_floor(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: floor(%f): %d, %lld ms\n", tb_scalar_to_float(x), r, t);
}
static tb_void_t tb_scalar_test_ceil(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_int_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_ceil(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: ceil(%f): %d, %lld ms\n", tb_scalar_to_float(x), r, t);
}
static tb_void_t tb_scalar_test_mul(tb_scalar_t a, tb_scalar_t b)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_scalar_t r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_mul(a, b);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: mul(%f, %f): %f, %lld ms\n", tb_scalar_to_float(a), tb_scalar_to_float(b), tb_scalar_to_float(r), t);

}
static tb_void_t tb_scalar_test_div(tb_scalar_t a, tb_scalar_t b)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_scalar_t r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_div(a, b);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: div(%f, %f): %f, %lld ms\n", tb_scalar_to_float(a), tb_scalar_to_float(b), tb_scalar_to_float(r), t);
}
static tb_void_t tb_scalar_test_invert(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_scalar_t	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_invert(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: invert(%f): %f, %lld ms\n", tb_scalar_to_float(x), tb_scalar_to_float(r), t);
}
static tb_void_t tb_scalar_test_sqre(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_scalar_t	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_sqre(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: sqre(%f): %f, %lld ms\n", tb_scalar_to_float(x), tb_scalar_to_float(r), t);
}
static tb_void_t tb_scalar_test_sqrt(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_scalar_t r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_sqrt(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: sqrt(%f) = %f, %lld ms\n", tb_scalar_to_float(x), tb_scalar_to_float(r), t);
}
static tb_void_t tb_scalar_test_ilog2(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_ilog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: ilog2(%f) = %d, %lld ms\n", tb_scalar_to_float(x), r, t);
}
static tb_void_t tb_scalar_test_iclog2(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_iclog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: iclog2(%f) = %d, %lld ms\n", tb_scalar_to_float(x), r, t);
}
static tb_void_t tb_scalar_test_irlog2(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_irlog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: irlog2(%f) = %d, %lld ms\n", tb_scalar_to_float(x), r, t);
}
static tb_void_t tb_scalar_test_exp(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_scalar_t r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_exp(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: exp(%f) = %f, %lld ms\n", tb_scalar_to_float(x), tb_scalar_to_float(r), t);
}
static tb_void_t tb_scalar_test_sin()
{
	__tb_volatile__ tb_int_t 	i = 0;
	__tb_volatile__ tb_int_t 	n = 10000000 / 360;
	__tb_volatile__ tb_scalar_t	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		for (i = 0; i < 360; i++)
			r = tb_scalar_sin(tb_scalar_test_angle[i]);
	}
	t = tb_mclock() - t;

	for (i = 0; i < 360; i++)
	{
		r = tb_scalar_sin(tb_scalar_test_angle[i]);
		tb_printf("[scalar]: sin(%f) = %f, angle: %d\n", tb_scalar_to_float(tb_scalar_test_angle[i]), tb_scalar_to_float(r), i);
	}
	tb_printf("[scalar]: sin(0 - 360), %lld ms\n", t);
}
static tb_void_t tb_scalar_test_cos()
{
	__tb_volatile__ tb_int_t 	i = 0;
	__tb_volatile__ tb_int_t 	n = 10000000 / 360;
	__tb_volatile__ tb_scalar_t	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		for (i = 0; i < 360; i++)
			r = tb_scalar_cos(tb_scalar_test_angle[i]);
	}
	t = tb_mclock() - t;

	for (i = 0; i < 360; i++)
	{
		r = tb_scalar_cos(tb_scalar_test_angle[i]);
		tb_printf("[scalar]: cos(%f) = %f, angle: %d\n", tb_scalar_to_float(tb_scalar_test_angle[i]), tb_scalar_to_float(r), i);
	}
	tb_printf("[scalar]: cos(0 - 360), %lld ms\n", t);
}
static tb_void_t tb_scalar_test_tan()
{
	__tb_volatile__ tb_int_t 	i = 0;
	__tb_volatile__ tb_int_t 	n = 10000000 / 360;
	__tb_volatile__ tb_scalar_t	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		for (i = 0; i < 360; i++)
			r = tb_scalar_tan(tb_scalar_test_angle[i]);
	}
	t = tb_mclock() - t;

	for (i = 0; i < 360; i++)
	{
		r = tb_scalar_tan(tb_scalar_test_angle[i]);
		tb_printf("[scalar]: tan(%f) = %f, angle: %d\n", tb_scalar_to_float(tb_scalar_test_angle[i]), tb_scalar_to_float(r), i);
	}
	tb_printf("[scalar]: tan(0 - 360), %lld ms\n", t);
}
static tb_void_t tb_scalar_test_atan2()
{
	__tb_volatile__ tb_int_t 	i = 0;
	__tb_volatile__ tb_int_t 	j = 0;
	__tb_volatile__ tb_int_t 	n = 10000000 / 100;
	__tb_volatile__ tb_scalar_t	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		for (i = 50; i < 55; i++)
		{
			for (j = 50; j < 55; j++)
			{
				r = tb_scalar_atan2(i, j);
				r = tb_scalar_atan2(i, -j);
				r = tb_scalar_atan2(-i, -j);
				r = tb_scalar_atan2(-i, j);
			}
		}
	}
	t = tb_mclock() - t;

	for (i = 50; i < 55; i++)
	{
		for (j = 50; j < 55; j++)
		{
			r = tb_scalar_atan2(i, j);
			tb_printf("[scalar]: atan2(%d, %d) = %f\n", i, j, tb_scalar_to_float(r));
			r = tb_scalar_atan2(i, -j);
			tb_printf("[scalar]: atan2(%d, %d) = %f\n", i, -j, tb_scalar_to_float(r));
			r = tb_scalar_atan2(-i, -j);
			tb_printf("[scalar]: atan2(%d, %d) = %f\n", -i, -j, tb_scalar_to_float(r));
			r = tb_scalar_atan2(-i, j);
			tb_printf("[scalar]: atan2(%d, %d) = %f\n", -i, j, tb_scalar_to_float(r));
		}
	}

	tb_printf("[scalar]: atan2(), %lld ms\n", t);
}

static tb_void_t tb_scalar_test_asin()
{
	__tb_volatile__ tb_int_t 	i = 0;
	__tb_volatile__ tb_int_t 	j = 0;
	__tb_volatile__ tb_int_t 	n = 10000000 / 50;
	__tb_volatile__ tb_scalar_t	r = 0;
	__tb_volatile__ tb_scalar_t	a = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		for (i = 0; i < 5; i++)
		{
			for (j = 0; j < 5; j++)
			{
				a = tb_scalar_test_arc[i][j];
				r = tb_scalar_asin(a);
				r = tb_scalar_asin(-a);
			}
		}
	}
	t = tb_mclock() - t;

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			a = tb_scalar_test_arc[i][j];
			r = tb_scalar_asin(a);
			tb_printf("[scalar]: asin(%f) = %f\n", tb_scalar_to_float(a), tb_scalar_to_float(r));
			r = tb_scalar_asin(-a);
			tb_printf("[scalar]: asin(%f) = %f\n", tb_scalar_to_float(-a), tb_scalar_to_float(r));
		}
	}

	tb_printf("[scalar]: asin, %lld ms\n", t);
}
static tb_void_t tb_scalar_test_acos()
{
	__tb_volatile__ tb_int_t 	i = 0;
	__tb_volatile__ tb_int_t 	j = 0;
	__tb_volatile__ tb_int_t 	n = 10000000 / 50;
	__tb_volatile__ tb_scalar_t r = 0;
	__tb_volatile__ tb_scalar_t a = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		for (i = 0; i < 5; i++)
		{
			for (j = 0; j < 5; j++)
			{
				a = tb_scalar_test_arc[i][j];
				r = tb_scalar_acos(a);
				r = tb_scalar_acos(-a);
			}
		}
	}
	t = tb_mclock() - t;

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			a = tb_scalar_test_arc[i][j];
			r = tb_scalar_acos(a);
			tb_printf("[scalar]: acos(%f) = %f\n", tb_scalar_to_float(a), tb_scalar_to_float(r));
			r = tb_scalar_acos(-a);
			tb_printf("[scalar]: acos(%f) = %f\n", tb_scalar_to_float(-a), tb_scalar_to_float(r));
		}
	}

	tb_printf("[scalar]: acos, %lld ms\n", t);
}
static tb_void_t tb_scalar_test_atan()
{
	__tb_volatile__ tb_int_t 	i = 0;
	__tb_volatile__ tb_int_t 	n = 10000000 / 100;
	__tb_volatile__ tb_scalar_t 	r = 0;
	__tb_volatile__ tb_scalar_t 	a = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		for (i = -50; i <= 50; i++)
		{
			a = tb_scalar_test_atan_a[i + 50];
			r = tb_scalar_atan(a);
		}
	}
	t = tb_mclock() - t;


	for (i = -50; i <= 50; i++)
	{
		a = tb_scalar_test_atan_a[i + 50];
		r = tb_scalar_atan(a);
		tb_printf("[scalar]: atan(%f) = %f\n", tb_scalar_to_float(a), tb_scalar_to_float(r));
	}

	tb_printf("[scalar]: atan, %lld ms\n", t);
}

#endif
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

#ifdef TB_CONFIG_TYPE_FLOAT
	tb_scalar_test_constant();
	tb_scalar_test_round(TB_SCALAR_PI);
	tb_scalar_test_round(-TB_SCALAR_PI);
	tb_scalar_test_floor(TB_SCALAR_PI);
	tb_scalar_test_floor(-TB_SCALAR_PI);
	tb_scalar_test_ceil(TB_SCALAR_PI);
	tb_scalar_test_ceil(-TB_SCALAR_PI);
	tb_scalar_test_mul(TB_SCALAR_PI, -TB_SCALAR_PI);
	tb_scalar_test_div(TB_SCALAR_ONE, TB_SCALAR_PI);
	tb_scalar_test_invert(TB_SCALAR_PI);
	tb_scalar_test_sqre(TB_SCALAR_PI);
	tb_scalar_test_sqre(-TB_SCALAR_PI);
	tb_scalar_test_sqrt(TB_SCALAR_PI);
	tb_scalar_test_exp(TB_SCALAR_PI);
	tb_scalar_test_ilog2(TB_SCALAR_PI);
	tb_scalar_test_ilog2(tb_float_to_scalar(1024.));
	tb_scalar_test_iclog2(TB_SCALAR_PI);
	tb_scalar_test_irlog2(TB_SCALAR_PI);
	tb_scalar_test_sin();
	tb_scalar_test_cos();
	tb_scalar_test_tan();
	tb_scalar_test_atan2();
	tb_scalar_test_asin();
	tb_scalar_test_acos();
	tb_scalar_test_atan();
#endif

	return 0;
}
