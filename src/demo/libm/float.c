/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_float_t tb_float_test_angle[360] =
{
    0.000000f
,   0.017453f
,   0.034907f
,   0.052360f
,   0.069813f
,   0.087266f
,   0.104720f
,   0.122173f
,   0.139626f
,   0.157080f
,   0.174533f
,   0.191986f
,   0.209440f
,   0.226893f
,   0.244346f
,   0.261799f
,   0.279253f
,   0.296706f
,   0.314159f
,   0.331613f
,   0.349066f
,   0.366519f
,   0.383972f
,   0.401426f
,   0.418879f
,   0.436332f
,   0.453786f
,   0.471239f
,   0.488692f
,   0.506145f
,   0.523599f
,   0.541052f
,   0.558505f
,   0.575959f
,   0.593412f
,   0.610865f
,   0.628319f
,   0.645772f
,   0.663225f
,   0.680678f
,   0.698132f
,   0.715585f
,   0.733038f
,   0.750492f
,   0.767945f
,   0.785398f
,   0.802851f
,   0.820305f
,   0.837758f
,   0.855211f
,   0.872665f
,   0.890118f
,   0.907571f
,   0.925025f
,   0.942478f
,   0.959931f
,   0.977384f
,   0.994838f
,   1.012291f
,   1.029744f
,   1.047198f
,   1.064651f
,   1.082104f
,   1.099557f
,   1.117011f
,   1.134464f
,   1.151917f
,   1.169371f
,   1.186824f
,   1.204277f
,   1.221730f
,   1.239184f
,   1.256637f
,   1.274090f
,   1.291544f
,   1.308997f
,   1.326450f
,   1.343904f
,   1.361357f
,   1.378810f
,   1.396263f
,   1.413717f
,   1.431170f
,   1.448623f
,   1.466077f
,   1.483530f
,   1.500983f
,   1.518436f
,   1.535890f
,   1.553343f
,   1.570796f
,   1.588250f
,   1.605703f
,   1.623156f
,   1.640609f
,   1.658063f
,   1.675516f
,   1.692969f
,   1.710423f
,   1.727876f
,   1.745329f
,   1.762783f
,   1.780236f
,   1.797689f
,   1.815142f
,   1.832596f
,   1.850049f
,   1.867502f
,   1.884956f
,   1.902409f
,   1.919862f
,   1.937315f
,   1.954769f
,   1.972222f
,   1.989675f
,   2.007129f
,   2.024582f
,   2.042035f
,   2.059489f
,   2.076942f
,   2.094395f
,   2.111848f
,   2.129302f
,   2.146755f
,   2.164208f
,   2.181662f
,   2.199115f
,   2.216568f
,   2.234021f
,   2.251475f
,   2.268928f
,   2.286381f
,   2.303835f
,   2.321288f
,   2.338741f
,   2.356194f
,   2.373648f
,   2.391101f
,   2.408554f
,   2.426008f
,   2.443461f
,   2.460914f
,   2.478368f
,   2.495821f
,   2.513274f
,   2.530727f
,   2.548181f
,   2.565634f
,   2.583087f
,   2.600541f
,   2.617994f
,   2.635447f
,   2.652900f
,   2.670354f
,   2.687807f
,   2.705260f
,   2.722714f
,   2.740167f
,   2.757620f
,   2.775074f
,   2.792527f
,   2.809980f
,   2.827433f
,   2.844887f
,   2.862340f
,   2.879793f
,   2.897247f
,   2.914700f
,   2.932153f
,   2.949606f
,   2.967060f
,   2.984513f
,   3.001966f
,   3.019420f
,   3.036873f
,   3.054326f
,   3.071779f
,   3.089233f
,   3.106686f
,   3.124139f
,   3.141593f
,   3.159046f
,   3.176499f
,   3.193953f
,   3.211406f
,   3.228859f
,   3.246312f
,   3.263766f
,   3.281219f
,   3.298672f
,   3.316126f
,   3.333579f
,   3.351032f
,   3.368485f
,   3.385939f
,   3.403392f
,   3.420845f
,   3.438299f
,   3.455752f
,   3.473205f
,   3.490659f
,   3.508112f
,   3.525565f
,   3.543018f
,   3.560472f
,   3.577925f
,   3.595378f
,   3.612832f
,   3.630285f
,   3.647738f
,   3.665191f
,   3.682645f
,   3.700098f
,   3.717551f
,   3.735005f
,   3.752458f
,   3.769911f
,   3.787364f
,   3.804818f
,   3.822271f
,   3.839724f
,   3.857178f
,   3.874631f
,   3.892084f
,   3.909538f
,   3.926991f
,   3.944444f
,   3.961897f
,   3.979351f
,   3.996804f
,   4.014257f
,   4.031711f
,   4.049164f
,   4.066617f
,   4.084070f
,   4.101524f
,   4.118977f
,   4.136430f
,   4.153884f
,   4.171337f
,   4.188790f
,   4.206243f
,   4.223697f
,   4.241150f
,   4.258603f
,   4.276057f
,   4.293510f
,   4.310963f
,   4.328417f
,   4.345870f
,   4.363323f
,   4.380776f
,   4.398230f
,   4.415683f
,   4.433136f
,   4.450590f
,   4.468043f
,   4.485496f
,   4.502949f
,   4.520403f
,   4.537856f
,   4.555309f
,   4.572763f
,   4.590216f
,   4.607669f
,   4.625123f
,   4.642576f
,   4.660029f
,   4.677482f
,   4.694936f
,   4.712389f
,   4.729842f
,   4.747296f
,   4.764749f
,   4.782202f
,   4.799655f
,   4.817109f
,   4.834562f
,   4.852015f
,   4.869469f
,   4.886922f
,   4.904375f
,   4.921828f
,   4.939282f
,   4.956735f
,   4.974188f
,   4.991642f
,   5.009095f
,   5.026548f
,   5.044002f
,   5.061455f
,   5.078908f
,   5.096361f
,   5.113815f
,   5.131268f
,   5.148721f
,   5.166175f
,   5.183628f
,   5.201081f
,   5.218534f
,   5.235988f
,   5.253441f
,   5.270894f
,   5.288348f
,   5.305801f
,   5.323254f
,   5.340708f
,   5.358161f
,   5.375614f
,   5.393067f
,   5.410521f
,   5.427974f
,   5.445427f
,   5.462881f
,   5.480334f
,   5.497787f
,   5.515240f
,   5.532694f
,   5.550147f
,   5.567600f
,   5.585054f
,   5.602507f
,   5.619960f
,   5.637413f
,   5.654867f
,   5.672320f
,   5.689773f
,   5.707227f
,   5.724680f
,   5.742133f
,   5.759587f
,   5.777040f
,   5.794493f
,   5.811946f
,   5.829400f
,   5.846853f
,   5.864306f
,   5.881760f
,   5.899213f
,   5.916666f
,   5.934119f
,   5.951573f
,   5.969026f
,   5.986479f
,   6.003933f
,   6.021386f
,   6.038839f
,   6.056293f
,   6.073746f
,   6.091199f
,   6.108652f
,   6.126106f
,   6.143559f
,   6.161012f
,   6.178466f
,   6.195919f
,   6.213372f
,   6.230825f
,   6.248279f
,   6.265732f

};

static tb_float_t tb_float_test_arc[5][4] =
{
    {0.000000f, 0.000000f, 0.000000f, 0.000000f}
,   {1.000000f, 0.500000f, 0.333333f, 0.250000f}
,   {2.000000f, 1.000000f, 0.666667f, 0.500000f}
,   {3.000000f, 1.500000f, 1.000000f, 0.750000f}
,   {4.000000f, 2.000000f, 1.333333f, 1.000000f}

};

static tb_float_t tb_float_test_atan_a[] =
{
    -10.000000f
,   -9.800000f
,   -9.600000f
,   -9.400000f
,   -9.200000f
,   -9.000000f
,   -8.800000f
,   -8.600000f
,   -8.400000f
,   -8.200000f
,   -8.000000f
,   -7.800000f
,   -7.600000f
,   -7.400000f
,   -7.200000f
,   -7.000000f
,   -6.800000f
,   -6.600000f
,   -6.400000f
,   -6.200000f
,   -6.000000f
,   -5.800000f
,   -5.600000f
,   -5.400000f
,   -5.200000f
,   -5.000000f
,   -4.800000f
,   -4.600000f
,   -4.400000f
,   -4.200000f
,   -4.000000f
,   -3.800000f
,   -3.600000f
,   -3.400000f
,   -3.200000f
,   -3.000000f
,   -2.800000f
,   -2.600000f
,   -2.400000f
,   -2.200000f
,   -2.000000f
,   -1.800000f
,   -1.600000f
,   -1.400000f
,   -1.200000f
,   -1.000000f
,   -0.800000f
,   -0.600000f
,   -0.400000f
,   -0.200000f
,   0.000000f
,   0.200000f
,   0.400000f
,   0.600000f
,   0.800000f
,   1.000000f
,   1.200000f
,   1.400000f
,   1.600000f
,   1.800000f
,   2.000000f
,   2.200000f
,   2.400000f
,   2.600000f
,   2.800000f
,   3.000000f
,   3.200000f
,   3.400000f
,   3.600000f
,   3.800000f
,   4.000000f
,   4.200000f
,   4.400000f
,   4.600000f
,   4.800000f
,   5.000000f
,   5.200000f
,   5.400000f
,   5.600000f
,   5.800000f
,   6.000000f
,   6.200000f
,   6.400000f
,   6.600000f
,   6.800000f
,   7.000000f
,   7.200000f
,   7.400000f
,   7.600000f
,   7.800000f
,   8.000000f
,   8.200000f
,   8.400000f
,   8.600000f
,   8.800000f
,   9.000000f
,   9.200000f
,   9.400000f
,   9.600000f
,   9.800000f
,   10.000000f
};
/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_float_test_constant()
{
    tb_printf("[float]: max = %f\n", (tb_float_t)TB_MAF);
    tb_printf("[float]: min = %f\n", (tb_float_t)TB_MIF);
    tb_printf("[float]: nan = %f\n", (tb_float_t)TB_NAN);
    tb_printf("[float]: inf = %f\n", (tb_float_t)TB_INF);
    tb_printf("[float]: isinf = %ld\n", tb_isinff((tb_float_t)TB_INF));
    tb_printf("[float]: isnan = %ld\n", tb_isnanf((tb_float_t)TB_NAN));
}
static tb_void_t tb_float_test_round(tb_float_t x)
{
    __tb_volatile__ tb_long_t       n = 10000000;
    __tb_volatile__ tb_long_t       r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_round(x);
    }
    t = tb_mclock() - t;
    tb_printf("[float]: round(%f): %ld, %lld ms\n", (x), r, t);
}
static tb_void_t tb_float_test_floor(tb_float_t x)
{
    __tb_volatile__ tb_long_t       n = 10000000;
    __tb_volatile__ tb_long_t       r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_floor(x);
    }
    t = tb_mclock() - t;
    tb_printf("[float]: floor(%f): %ld, %lld ms\n", (x), r, t);
}
static tb_void_t tb_float_test_ceil(tb_float_t x)
{
    __tb_volatile__ tb_long_t       n = 10000000;
    __tb_volatile__ tb_long_t       r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_ceil(x);
    }
    t = tb_mclock() - t;
    tb_printf("[float]: ceil(%f): %ld, %lld ms\n", (x), r, t);
}
static tb_void_t tb_float_test_mul(tb_float_t a, tb_float_t b)
{
    __tb_volatile__ tb_long_t       n = 10000000;
    __tb_volatile__ tb_float_t      r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = a * b;
    }
    t = tb_mclock() - t;
    tb_printf("[float]: mul(%f, %f): %f, %lld ms\n", (a), (b), (r), t);

}
static tb_void_t tb_float_test_div(tb_float_t a, tb_float_t b)
{
    __tb_volatile__ tb_long_t       n = 10000000;
    __tb_volatile__ tb_float_t      r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = a / b;
    }
    t = tb_mclock() - t;
    tb_printf("[float]: div(%f, %f): %f, %lld ms\n", (a), (b), (r), t);
}
static tb_void_t tb_float_test_sqrt(tb_float_t x)
{
    __tb_volatile__ tb_long_t       n = 10000000;
    __tb_volatile__ tb_float_t      r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_sqrtf(x);
    }
    t = tb_mclock() - t;
    tb_printf("[float]: sqrt(%f) = %f, %lld ms\n", (x), (r), t);
}
static tb_void_t tb_float_test_exp(tb_float_t x)
{
    __tb_volatile__ tb_long_t       n = 10000000;
    __tb_volatile__ tb_float_t      r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_expf(x);
    }
    t = tb_mclock() - t;
    tb_printf("[float]: exp(%f) = %f, %lld ms\n", (x), r, t);
}

static tb_void_t tb_float_test_sin()
{
#if 1
    __tb_volatile__ tb_long_t       i = 0;
    __tb_volatile__ tb_long_t       n = 10000000 / 360;
    __tb_volatile__ tb_float_t      r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = 0; i < 360; i++)
            r = tb_sinf(tb_float_test_angle[i]);
    }
    t = tb_mclock() - t;

    for (i = 0; i < 360; i++)
    {
        r = tb_sinf(tb_float_test_angle[i]);
        tb_printf("[float]: sin(%f) = %f, angle: %ld\n", (tb_float_test_angle[i]), (r), i);
    }
    tb_printf("[float]: sin(0 - 360), %lld ms\n", t);
#else

    tb_long_t i = 0;
    for (i = 0; i < 360; i++)
    {
        tb_printf(",\t%f\n", (i * TB_PI / 180));
    }
#endif
}

static tb_void_t tb_float_test_cos()
{
    __tb_volatile__ tb_long_t       i = 0;
    __tb_volatile__ tb_long_t       n = 10000000 / 360;
    __tb_volatile__ tb_float_t  r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = 0; i < 360; i++)
            r = tb_cosf(tb_float_test_angle[i]);
    }
    t = tb_mclock() - t;

    for (i = 0; i < 360; i++)
    {
        r = tb_cosf(tb_float_test_angle[i]);
        tb_printf("[float]: cos(%f) = %f, angle: %ld\n", (tb_float_test_angle[i]), (r), i);
    }
    tb_printf("[float]: cos(0 - 360), %lld ms\n", t);
}

static tb_void_t tb_float_test_tan()
{
    __tb_volatile__ tb_long_t       i = 0;
    __tb_volatile__ tb_long_t       n = 10000000 / 360;
    __tb_volatile__ tb_float_t      r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = 0; i < 360; i++)
            r = tb_tanf(tb_float_test_angle[i]);
    }
    t = tb_mclock() - t;

    for (i = 0; i < 360; i++)
    {
        r = tb_tanf(tb_float_test_angle[i]);
        tb_printf("[float]: tan(%f) = %f, angle: %ld\n", (tb_float_test_angle[i]), (r), i);
    }
    tb_printf("[float]: tan(0 - 360), %lld ms\n", t);
}

static tb_void_t tb_float_test_atan2()
{
    __tb_volatile__ tb_long_t       i = 0;
    __tb_volatile__ tb_long_t       j = 0;
    __tb_volatile__ tb_long_t       n = 10000000 / 100;
    __tb_volatile__ tb_float_t      r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = 50; i < 55; i++)
        {
            for (j = 50; j < 55; j++)
            {
                r = tb_atan2f((tb_float_t)i, (tb_float_t)j);
                r = tb_atan2f((tb_float_t)i, (tb_float_t)-j);
                r = tb_atan2f((tb_float_t)-i, (tb_float_t)-j);
                r = tb_atan2f((tb_float_t)-i, (tb_float_t)j);
            }
        }
    }
    t = tb_mclock() - t;

    for (i = 50; i < 55; i++)
    {
        for (j = 50; j < 55; j++)
        {
            r = tb_atan2f((tb_float_t)i, (tb_float_t)j);
            tb_printf("[float]: atan2(%ld, %ld) = %f\n", i, j, (r));
            r = tb_atan2f((tb_float_t)i, (tb_float_t)-j);
            tb_printf("[float]: atan2(%ld, %ld) = %f\n", i, -j, (r));
            r = tb_atan2f((tb_float_t)-i, (tb_float_t)-j);
            tb_printf("[float]: atan2(%ld, %ld) = %f\n", -i, -j, (r));
            r = tb_atan2f((tb_float_t)-i, (tb_float_t)j);
            tb_printf("[float]: atan2(%ld, %ld) = %f\n", -i, j, (r));
        }
    }

    tb_printf("[float]: atan2(), %lld ms\n", t);
}

static tb_void_t tb_float_test_asin()
{
    __tb_volatile__ tb_long_t       i = 0;
    __tb_volatile__ tb_long_t       j = 0;
    __tb_volatile__ tb_long_t       n = 10000000 / 50;
    __tb_volatile__ tb_float_t      r = 0;
    __tb_volatile__ tb_float_t      a = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = 0; i < 5; i++)
        {
            for (j = 0; j < 4; j++)
            {
                a = tb_float_test_arc[i][j];
                r = tb_asinf(a);
                r = tb_asinf(-a);
            }
        }
    }
    t = tb_mclock() - t;

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 4; j++)
        {
            a = tb_float_test_arc[i][j];
            r = tb_asinf(a);
            tb_printf("[float]: asin(%f) = %f\n", (a), (r));
            r = tb_asinf(-a);
            tb_printf("[float]: asin(%f) = %f\n", (-a), (r));
        }
    }

    tb_printf("[float]: asin, %lld ms\n", t);
}
static tb_void_t tb_float_test_acos()
{
    __tb_volatile__ tb_long_t       i = 0;
    __tb_volatile__ tb_long_t       j = 0;
    __tb_volatile__ tb_long_t       n = 10000000 / 50;
    __tb_volatile__ tb_float_t      r = 0;
    __tb_volatile__ tb_float_t      a = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = 0; i < 5; i++)
        {
            for (j = 0; j < 5; j++)
            {
                a = tb_float_test_arc[i][j];
                r = tb_acosf(a);
                r = tb_acosf(-a);
            }
        }
    }
    t = tb_mclock() - t;

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 5; j++)
        {
            a = tb_float_test_arc[i][j];
            r = tb_acosf(a);
            tb_printf("[float]: acos(%f) = %f\n", (a), (r));
            r = tb_acosf(-a);
            tb_printf("[float]: acos(%f) = %f\n", (-a), (r));
        }
    }

    tb_printf("[float]: acos, %lld ms\n", t);
}
static tb_void_t tb_float_test_atan()
{
    __tb_volatile__ tb_long_t   i = 0;
    __tb_volatile__ tb_long_t   n = 10000000 / 100;
    __tb_volatile__ tb_float_t  r = 0;
    __tb_volatile__ tb_float_t  a = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        for (i = -50; i <= 50; i++)
        {
            a = tb_float_test_atan_a[i + 50];
            r = tb_atanf(a);
        }
    }
    t = tb_mclock() - t;

    for (i = -50; i <= 50; i++)
    {
        a = tb_float_test_atan_a[i + 50];
        r = tb_atanf(a);
        tb_printf("[float]: atan(%f) = %f\n", (a), (r));
    }

    tb_printf("[float]: atan, %lld ms\n", t);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_libm_float_main(tb_int_t argc, tb_char_t** argv)
{
    tb_float_test_constant();
    tb_float_test_round((tb_float_t)TB_PI);
    tb_float_test_round((tb_float_t)-TB_PI);
    tb_float_test_floor((tb_float_t)TB_PI);
    tb_float_test_floor((tb_float_t)-TB_PI);
    tb_float_test_ceil((tb_float_t)TB_PI);
    tb_float_test_ceil((tb_float_t)-TB_PI);
    tb_float_test_mul((tb_float_t)TB_PI, (tb_float_t)-TB_PI);
    tb_float_test_div(1., (tb_float_t)TB_PI);
    tb_float_test_sqrt((tb_float_t)TB_PI);
    tb_float_test_exp((tb_float_t)TB_PI);
    tb_float_test_sin();
    tb_float_test_cos();
    tb_float_test_tan();
    tb_float_test_atan2();
    tb_float_test_asin();
    tb_float_test_acos();
    tb_float_test_atan();

    return 0;
}

