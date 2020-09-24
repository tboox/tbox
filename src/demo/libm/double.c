/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_double_t tb_double_test_angle[360] =
{
    0.000000
,   0.017453
,   0.034907
,   0.052360
,   0.069813
,   0.087266
,   0.104720
,   0.122173
,   0.139626
,   0.157080
,   0.174533
,   0.191986
,   0.209440
,   0.226893
,   0.244346
,   0.261799
,   0.279253
,   0.296706
,   0.314159
,   0.331613
,   0.349066
,   0.366519
,   0.383972
,   0.401426
,   0.418879
,   0.436332
,   0.453786
,   0.471239
,   0.488692
,   0.506145
,   0.523599
,   0.541052
,   0.558505
,   0.575959
,   0.593412
,   0.610865
,   0.628319
,   0.645772
,   0.663225
,   0.680678
,   0.698132
,   0.715585
,   0.733038
,   0.750492
,   0.767945
,   0.785398
,   0.802851
,   0.820305
,   0.837758
,   0.855211
,   0.872665
,   0.890118
,   0.907571
,   0.925025
,   0.942478
,   0.959931
,   0.977384
,   0.994838
,   1.012291
,   1.029744
,   1.047198
,   1.064651
,   1.082104
,   1.099557
,   1.117011
,   1.134464
,   1.151917
,   1.169371
,   1.186824
,   1.204277
,   1.221730
,   1.239184
,   1.256637
,   1.274090
,   1.291544
,   1.308997
,   1.326450
,   1.343904
,   1.361357
,   1.378810
,   1.396263
,   1.413717
,   1.431170
,   1.448623
,   1.466077
,   1.483530
,   1.500983
,   1.518436
,   1.535890
,   1.553343
,   1.570796
,   1.588250
,   1.605703
,   1.623156
,   1.640609
,   1.658063
,   1.675516
,   1.692969
,   1.710423
,   1.727876
,   1.745329
,   1.762783
,   1.780236
,   1.797689
,   1.815142
,   1.832596
,   1.850049
,   1.867502
,   1.884956
,   1.902409
,   1.919862
,   1.937315
,   1.954769
,   1.972222
,   1.989675
,   2.007129
,   2.024582
,   2.042035
,   2.059489
,   2.076942
,   2.094395
,   2.111848
,   2.129302
,   2.146755
,   2.164208
,   2.181662
,   2.199115
,   2.216568
,   2.234021
,   2.251475
,   2.268928
,   2.286381
,   2.303835
,   2.321288
,   2.338741
,   2.356194
,   2.373648
,   2.391101
,   2.408554
,   2.426008
,   2.443461
,   2.460914
,   2.478368
,   2.495821
,   2.513274
,   2.530727
,   2.548181
,   2.565634
,   2.583087
,   2.600541
,   2.617994
,   2.635447
,   2.652900
,   2.670354
,   2.687807
,   2.705260
,   2.722714
,   2.740167
,   2.757620
,   2.775074
,   2.792527
,   2.809980
,   2.827433
,   2.844887
,   2.862340
,   2.879793
,   2.897247
,   2.914700
,   2.932153
,   2.949606
,   2.967060
,   2.984513
,   3.001966
,   3.019420
,   3.036873
,   3.054326
,   3.071779
,   3.089233
,   3.106686
,   3.124139
,   3.141593
,   3.159046
,   3.176499
,   3.193953
,   3.211406
,   3.228859
,   3.246312
,   3.263766
,   3.281219
,   3.298672
,   3.316126
,   3.333579
,   3.351032
,   3.368485
,   3.385939
,   3.403392
,   3.420845
,   3.438299
,   3.455752
,   3.473205
,   3.490659
,   3.508112
,   3.525565
,   3.543018
,   3.560472
,   3.577925
,   3.595378
,   3.612832
,   3.630285
,   3.647738
,   3.665191
,   3.682645
,   3.700098
,   3.717551
,   3.735005
,   3.752458
,   3.769911
,   3.787364
,   3.804818
,   3.822271
,   3.839724
,   3.857178
,   3.874631
,   3.892084
,   3.909538
,   3.926991
,   3.944444
,   3.961897
,   3.979351
,   3.996804
,   4.014257
,   4.031711
,   4.049164
,   4.066617
,   4.084070
,   4.101524
,   4.118977
,   4.136430
,   4.153884
,   4.171337
,   4.188790
,   4.206243
,   4.223697
,   4.241150
,   4.258603
,   4.276057
,   4.293510
,   4.310963
,   4.328417
,   4.345870
,   4.363323
,   4.380776
,   4.398230
,   4.415683
,   4.433136
,   4.450590
,   4.468043
,   4.485496
,   4.502949
,   4.520403
,   4.537856
,   4.555309
,   4.572763
,   4.590216
,   4.607669
,   4.625123
,   4.642576
,   4.660029
,   4.677482
,   4.694936
,   4.712389
,   4.729842
,   4.747296
,   4.764749
,   4.782202
,   4.799655
,   4.817109
,   4.834562
,   4.852015
,   4.869469
,   4.886922
,   4.904375
,   4.921828
,   4.939282
,   4.956735
,   4.974188
,   4.991642
,   5.009095
,   5.026548
,   5.044002
,   5.061455
,   5.078908
,   5.096361
,   5.113815
,   5.131268
,   5.148721
,   5.166175
,   5.183628
,   5.201081
,   5.218534
,   5.235988
,   5.253441
,   5.270894
,   5.288348
,   5.305801
,   5.323254
,   5.340708
,   5.358161
,   5.375614
,   5.393067
,   5.410521
,   5.427974
,   5.445427
,   5.462881
,   5.480334
,   5.497787
,   5.515240
,   5.532694
,   5.550147
,   5.567600
,   5.585054
,   5.602507
,   5.619960
,   5.637413
,   5.654867
,   5.672320
,   5.689773
,   5.707227
,   5.724680
,   5.742133
,   5.759587
,   5.777040
,   5.794493
,   5.811946
,   5.829400
,   5.846853
,   5.864306
,   5.881760
,   5.899213
,   5.916666
,   5.934119
,   5.951573
,   5.969026
,   5.986479
,   6.003933
,   6.021386
,   6.038839
,   6.056293
,   6.073746
,   6.091199
,   6.108652
,   6.126106
,   6.143559
,   6.161012
,   6.178466
,   6.195919
,   6.213372
,   6.230825
,   6.248279
,   6.265732

};

static tb_double_t tb_double_test_arc[5][4] =
{
    {0.000000, 0.000000, 0.000000, 0.000000}
,   {1.000000, 0.500000, 0.333333, 0.250000}
,   {2.000000, 1.000000, 0.666667, 0.500000}
,   {3.000000, 1.500000, 1.000000, 0.750000}
,   {4.000000, 2.000000, 1.333333, 1.000000}

};

static tb_double_t tb_double_test_atan_a[] =
{
    -10.000000
,   -9.800000
,   -9.600000
,   -9.400000
,   -9.200000
,   -9.000000
,   -8.800000
,   -8.600000
,   -8.400000
,   -8.200000
,   -8.000000
,   -7.800000
,   -7.600000
,   -7.400000
,   -7.200000
,   -7.000000
,   -6.800000
,   -6.600000
,   -6.400000
,   -6.200000
,   -6.000000
,   -5.800000
,   -5.600000
,   -5.400000
,   -5.200000
,   -5.000000
,   -4.800000
,   -4.600000
,   -4.400000
,   -4.200000
,   -4.000000
,   -3.800000
,   -3.600000
,   -3.400000
,   -3.200000
,   -3.000000
,   -2.800000
,   -2.600000
,   -2.400000
,   -2.200000
,   -2.000000
,   -1.800000
,   -1.600000
,   -1.400000
,   -1.200000
,   -1.000000
,   -0.800000
,   -0.600000
,   -0.400000
,   -0.200000
,   0.000000
,   0.200000
,   0.400000
,   0.600000
,   0.800000
,   1.000000
,   1.200000
,   1.400000
,   1.600000
,   1.800000
,   2.000000
,   2.200000
,   2.400000
,   2.600000
,   2.800000
,   3.000000
,   3.200000
,   3.400000
,   3.600000
,   3.800000
,   4.000000
,   4.200000
,   4.400000
,   4.600000
,   4.800000
,   5.000000
,   5.200000
,   5.400000
,   5.600000
,   5.800000
,   6.000000
,   6.200000
,   6.400000
,   6.600000
,   6.800000
,   7.000000
,   7.200000
,   7.400000
,   7.600000
,   7.800000
,   8.000000
,   8.200000
,   8.400000
,   8.600000
,   8.800000
,   9.000000
,   9.200000
,   9.400000
,   9.600000
,   9.800000
,   10.000000
};

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_double_test_constant()
{
    tb_printf("[double]: max = %lf\n", TB_MAF);
    tb_printf("[double]: min = %lf\n", TB_MIF);
    tb_printf("[double]: nan = %lf\n", TB_NAN);
    tb_printf("[double]: inf = %lf\n", TB_INF);
    tb_printf("[double]: isinf = %ld\n", tb_isinf(TB_INF));
    tb_printf("[double]: isnan = %ld\n", tb_isnan(TB_NAN));
}
static tb_void_t tb_double_test_round(tb_double_t x)
{
    __tb_volatile__ tb_long_t   n = 10000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_round(x);
    }
    t = tb_mclock() - t;
    tb_printf("[double]: round(%lf): %ld, %lld ms\n", (x), r, t);
}
static tb_void_t tb_double_test_floor(tb_double_t x)
{
    __tb_volatile__ tb_long_t   n = 10000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_floor(x);
    }
    t = tb_mclock() - t;
    tb_printf("[double]: floor(%lf): %ld, %lld ms\n", (x), r, t);
}
static tb_void_t tb_double_test_ceil(tb_double_t x)
{
    __tb_volatile__ tb_long_t   n = 10000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_ceil(x);
    }
    t = tb_mclock() - t;
    tb_printf("[double]: ceil(%lf): %ld, %lld ms\n", (x), r, t);
}
static tb_void_t tb_double_test_mul(tb_double_t a, tb_double_t b)
{
    __tb_volatile__ tb_long_t   n = 10000000;
    __tb_volatile__ tb_double_t     r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = a * b;
    }
    t = tb_mclock() - t;
    tb_printf("[double]: mul(%lf, %lf): %lf, %lld ms\n", (a), (b), (r), t);

}
static tb_void_t tb_double_test_div(tb_double_t a, tb_double_t b)
{
    __tb_volatile__ tb_long_t       n = 10000000;
    __tb_volatile__ tb_double_t     r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = a / b;
    }
    t = tb_mclock() - t;
    tb_printf("[double]: div(%lf, %lf): %lf, %lld ms\n", (a), (b), (r), t);
}
static tb_void_t tb_double_test_sqrt(tb_double_t x)
{
    __tb_volatile__ tb_long_t   n = 10000000;
    __tb_volatile__ tb_double_t     r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_sqrt(x);
    }
    t = tb_mclock() - t;
    tb_printf("[double]: sqrt(%lf) = %lf, %lld ms\n", (x), (r), t);
}
static tb_void_t tb_double_test_exp(tb_double_t x)
{
    __tb_volatile__ tb_long_t       n = 10000000;
    __tb_volatile__ tb_double_t     r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_exp(x);
    }
    t = tb_mclock() - t;
    tb_printf("[double]: exp(%lf) = %lf, %lld ms\n", (x), r, t);
}

static tb_void_t tb_double_test_sin()
{
#if 1
    __tb_volatile__ tb_long_t       i = 0;
    __tb_volatile__ tb_long_t       n = 10000000 / 360;
    __tb_volatile__ tb_double_t     r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = 0; i < 360; i++)
            r = tb_sin(tb_double_test_angle[i]);
    }
    t = tb_mclock() - t;

    for (i = 0; i < 360; i++)
    {
        r = tb_sin(tb_double_test_angle[i]);
        tb_printf("[double]: sin(%lf) = %lf, angle: %ld\n", (tb_double_test_angle[i]), (r), i);
    }
    tb_printf("[double]: sin(0 - 360), %lld ms\n", t);
#else

    tb_long_t i = 0;
    for (i = 0; i < 360; i++)
    {
        tb_printf(",\t%lf\n", (i * TB_PI / 180));
    }
#endif
}

static tb_void_t tb_double_test_cos()
{
    __tb_volatile__ tb_long_t       i = 0;
    __tb_volatile__ tb_long_t       n = 10000000 / 360;
    __tb_volatile__ tb_double_t     r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = 0; i < 360; i++)
            r = tb_cos(tb_double_test_angle[i]);
    }
    t = tb_mclock() - t;

    for (i = 0; i < 360; i++)
    {
        r = tb_cos(tb_double_test_angle[i]);
        tb_printf("[double]: cos(%lf) = %lf, angle: %ld\n", (tb_double_test_angle[i]), (r), i);
    }
    tb_printf("[double]: cos(0 - 360), %lld ms\n", t);
}

static tb_void_t tb_double_test_tan()
{
    __tb_volatile__ tb_long_t       i = 0;
    __tb_volatile__ tb_long_t       n = 10000000 / 360;
    __tb_volatile__ tb_double_t     r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = 0; i < 360; i++)
            r = tb_tan(tb_double_test_angle[i]);
    }
    t = tb_mclock() - t;

    for (i = 0; i < 360; i++)
    {
        r = tb_tan(tb_double_test_angle[i]);
        tb_printf("[double]: tan(%lf) = %lf, angle: %ld\n", (tb_double_test_angle[i]), (r), i);
    }
    tb_printf("[double]: tan(0 - 360), %lld ms\n", t);
}

static tb_void_t tb_double_test_atan2()
{
    __tb_volatile__ tb_double_t     i = 0;
    __tb_volatile__ tb_double_t     j = 0;
    __tb_volatile__ tb_long_t       n = 10000000 / 100;
    __tb_volatile__ tb_double_t     r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = 50.; i < 55.; i++)
        {
            for (j = 50.; j < 55.; j++)
            {
                r = tb_atan2(i, j);
                r = tb_atan2(i, -j);
                r = tb_atan2(-i, -j);
                r = tb_atan2(-i, j);
            }
        }
    }
    t = tb_mclock() - t;

    for (i = 50.; i < 55.; i++)
    {
        for (j = 50.; j < 55.; j++)
        {
            r = tb_atan2(i, j);
            tb_printf("[double]: atan2(%ld, %ld) = %lf\n", i, j, (r));
            r = tb_atan2(i, -j);
            tb_printf("[double]: atan2(%ld, %ld) = %lf\n", i, -j, (r));
            r = tb_atan2(-i, -j);
            tb_printf("[double]: atan2(%ld, %ld) = %lf\n", -i, -j, (r));
            r = tb_atan2(-i, j);
            tb_printf("[double]: atan2(%ld, %ld) = %lf\n", -i, j, (r));
        }
    }

    tb_printf("[double]: atan2(), %lld ms\n", t);
}

static tb_void_t tb_double_test_asin()
{
    __tb_volatile__ tb_long_t       i = 0;
    __tb_volatile__ tb_long_t       j = 0;
    __tb_volatile__ tb_long_t       n = 10000000 / 50;
    __tb_volatile__ tb_double_t     r = 0;
    __tb_volatile__ tb_double_t     a = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = 0; i < 5; i++)
        {
            for (j = 0; j < 4; j++)
            {
                a = tb_double_test_arc[i][j];
                r = tb_asin(a);
                r = tb_asin(-a);
            }
        }
    }
    t = tb_mclock() - t;

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 4; j++)
        {
            a = tb_double_test_arc[i][j];
            r = tb_asin(a);
            tb_printf("[double]: asin(%lf) = %lf\n", (a), (r));
            r = tb_asin(-a);
            tb_printf("[double]: asin(%lf) = %lf\n", (-a), (r));
        }
    }

    tb_printf("[double]: asin, %lld ms\n", t);
}
static tb_void_t tb_double_test_acos()
{
    __tb_volatile__ tb_long_t       i = 0;
    __tb_volatile__ tb_long_t       j = 0;
        __tb_volatile__ tb_long_t   n = 10000000 / 50;
    __tb_volatile__ tb_double_t     r = 0;
    __tb_volatile__ tb_double_t     a = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = 0; i < 5; i++)
        {
            for (j = 0; j < 5; j++)
            {
                a = tb_double_test_arc[i][j];
                r = tb_acos(a);
                r = tb_acos(-a);
            }
        }
    }
    t = tb_mclock() - t;

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 5; j++)
        {
            a = tb_double_test_arc[i][j];
            r = tb_acos(a);
            tb_printf("[double]: acos(%lf) = %lf\n", (a), (r));
            r = tb_acos(-a);
            tb_printf("[double]: acos(%lf) = %lf\n", (-a), (r));
        }
    }

    tb_printf("[double]: acos, %lld ms\n", t);
}
static tb_void_t tb_double_test_atan()
{
    __tb_volatile__ tb_long_t       i = 0;
    __tb_volatile__ tb_long_t       n = 10000000 / 100;
    __tb_volatile__ tb_double_t     r = 0;
    __tb_volatile__ tb_double_t     a = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = -50; i <= 50; i++)
        {
            a = tb_double_test_atan_a[i + 50];
            r = tb_atan(a);
        }
    }
    t = tb_mclock() - t;

    for (i = -50; i <= 50; i++)
    {
        a = tb_double_test_atan_a[i + 50];
        r = tb_atan(a);
        tb_printf("[double]: atan(%lf) = %lf\n", (a), (r));
    }

    tb_printf("[double]: atan, %lld ms\n", t);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_libm_double_main(tb_int_t argc, tb_char_t** argv)
{
    tb_double_test_constant();
    tb_double_test_round(TB_PI);
    tb_double_test_round(-TB_PI);
    tb_double_test_floor(TB_PI);
    tb_double_test_floor(-TB_PI);
    tb_double_test_ceil(TB_PI);
    tb_double_test_ceil(-TB_PI);
    tb_double_test_mul(TB_PI, -TB_PI);
    tb_double_test_div(1., TB_PI);
    tb_double_test_sqrt(TB_PI);
    tb_double_test_exp(TB_PI);
    tb_double_test_sin();
    tb_double_test_cos();
    tb_double_test_tan();
    tb_double_test_atan2();
    tb_double_test_asin();
    tb_double_test_acos();
    tb_double_test_atan();

    return 0;
}

