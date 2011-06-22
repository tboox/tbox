#include "tbox.h"

#ifdef TB_CONFIG_TYPE_FLOAT

static tb_float_t tb_float_test_angle[360] = 
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
static void tb_float_test_constant()
{
	tb_printf("[float]: one = %f\n", tb_float_to_float(TB_FLOAT_ONE));
	tb_printf("[float]: half = %f\n", tb_float_to_float(TB_FLOAT_HALF));
	tb_printf("[float]: max = %f\n", tb_float_to_float(TB_FLOAT_MAX));
	tb_printf("[float]: min = %f\n", tb_float_to_float(TB_FLOAT_MIN));
	tb_printf("[float]: nan = %f\n", tb_float_to_float(TB_FLOAT_NAN));
	tb_printf("[float]: inf = %f\n", tb_float_to_float(TB_FLOAT_INF));
	tb_printf("[float]: pi = %f\n", tb_float_to_float(TB_FLOAT_PI));
	tb_printf("[float]: sqrt2 = %f\n", tb_float_to_float(TB_FLOAT_SQRT2));
}
static void tb_float_test_round(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_int_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_round(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: round(%f): %d, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
}
static void tb_float_test_floor(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_int_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_floor(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: floor(%f): %d, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
}
static void tb_float_test_ceil(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_int_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_ceil(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: ceil(%f): %d, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
}
static void tb_float_test_mul(tb_float_t a, tb_float_t b)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_mul(a, b);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: mul(%f, %f): %f, %d ms\n", tb_float_to_float(a), tb_float_to_float(b), tb_float_to_float(r), (tb_int_t)t);

}
static void tb_float_test_div(tb_float_t a, tb_float_t b)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_div(a, b);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: div(%f, %f): %f, %d ms\n", tb_float_to_float(a), tb_float_to_float(b), tb_float_to_float(r), (tb_int_t)t);
}
static void tb_float_test_invert(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_invert(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: invert(%f): %f, %d ms\n", tb_float_to_float(x), tb_float_to_float(r), (tb_int_t)t);
}
static void tb_float_test_sqre(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_sqre(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: sqre(%f): %f, %d ms\n", tb_float_to_float(x), tb_float_to_float(r), (tb_int_t)t);
}
static void tb_float_test_sqrt(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_sqrt(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: sqrt(%f) = %f, %d ms\n", tb_float_to_float(x), tb_float_to_float(r), (tb_int_t)t);
}
static void tb_float_test_ilog2(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_ilog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: ilog2(%f) = %d, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
}
static void tb_float_test_iclog2(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_iclog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: iclog2(%f) = %d, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
}
static void tb_float_test_irlog2(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_irlog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: irlog2(%f) = %d, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
}
static void tb_float_test_exp(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_exp(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: exp(%f) = %f, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
}
#endif

static void tb_float_test_sin()
{
#if 1
	__tb_volatile__ tb_int_t 	i = 0;
	__tb_volatile__ tb_int_t 	n = 10000000 / 360;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		for (i = 0; i < 360; i++)
			r = tb_float_sin(tb_float_test_angle[i]);
	}
	t = tb_mclock() - t;

	for (i = 0; i < 360; i++)
	{
		r = tb_float_sin(tb_float_test_angle[i]);
		tb_printf("[float]: sin(%f) = %f, angle: %d\n", tb_float_to_float(tb_float_test_angle[i]), tb_float_to_float(r), i);
	}
	tb_printf("[float]: sin(0 - 360), %d ms\n", (tb_int_t)t);
#else

	tb_int_t i = 0;
	for (i = 0; i < 360; i++)
	{
		tb_printf(",\t%f\n", tb_float_to_float(i * TB_FLOAT_PI / 180));
	}
#endif
}

static void tb_float_test_cos()
{
	__tb_volatile__ tb_int_t 	i = 0;
	__tb_volatile__ tb_int_t 	n = 10000000 / 360;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		for (i = 0; i < 360; i++)
			r = tb_float_cos(tb_float_test_angle[i]);
	}
	t = tb_mclock() - t;

	for (i = 0; i < 360; i++)
	{
		r = tb_float_cos(tb_float_test_angle[i]);
		tb_printf("[float]: cos(%f) = %f, angle: %d\n", tb_float_to_float(tb_float_test_angle[i]), tb_float_to_float(r), i);
	}
	tb_printf("[float]: cos(0 - 360), %d ms\n", (tb_int_t)t);
}

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

#ifdef TB_CONFIG_TYPE_FLOAT
	tb_float_test_constant();
	tb_float_test_round(TB_FLOAT_PI);
	tb_float_test_round(-TB_FLOAT_PI);
	tb_float_test_floor(TB_FLOAT_PI);
	tb_float_test_floor(-TB_FLOAT_PI);
	tb_float_test_ceil(TB_FLOAT_PI);
	tb_float_test_ceil(-TB_FLOAT_PI);
	tb_float_test_mul(TB_FLOAT_PI, -TB_FLOAT_PI);
	tb_float_test_div(TB_FLOAT_ONE, TB_FLOAT_PI);
	tb_float_test_invert(TB_FLOAT_PI);
	tb_float_test_sqre(TB_FLOAT_PI);
	tb_float_test_sqre(-TB_FLOAT_PI);
	tb_float_test_sqrt(TB_FLOAT_PI);
	tb_float_test_exp(TB_FLOAT_PI);
	tb_float_test_ilog2(TB_FLOAT_PI);
	tb_float_test_ilog2(1024.0);
	tb_float_test_iclog2(TB_FLOAT_PI);
	tb_float_test_irlog2(TB_FLOAT_PI);
	tb_float_test_sin();
	tb_float_test_cos();
#endif

	return 0;
}

