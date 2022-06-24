#include <iostream>
#include <iomanip>
#include <cstring>
#include <chrono>
#include <cassert>
#include <vector>
#include <pthread.h>
#include <cmath>
#include <memory>
#include <tuple>
#include <numeric>
#include <string>
#include <immintrin.h>

// $CXX -O3 -mavx matmul-assignment.cpp

#ifdef __ORBIS__
// Only needed on the PS4: set to something sufficiently large.
extern "C" size_t sceLibcHeapSize = 256 * 1024 * 1024; // 256 MiB
#endif

#if (!defined(_MSC_VER))
#pragma GCC   diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wc++17-extensions"
#endif

#define SZ (1 << 3) // (1 << 10) //== 1024
#define SZ2 (1 << 6)

float simd_vreduce_f(const float*  a, const size_t size) {
    //float sum;
    __m128 vsum = _mm_set1_ps(0.0f);

    for (int i = 0; i < size; i += 4 /*sizeof(vsum) / *a*/) {
        __m128 v = _mm_load_ps(&a[i]);
        vsum = _mm_add_ps(vsum, v);
    }

    vsum = _mm_hadd_ps(vsum, vsum);
    vsum = _mm_hadd_ps(vsum, vsum);

    return _mm_cvtss_f32(vsum);
}

double simd_vreduce_d(const double* a, const size_t size) {
    __m256d vsum = _mm256_set1_pd(0);

    for (std::size_t i = 0; i < size; i += 4) {
        __m256d v = _mm256_load_pd(&a[i]);
        vsum = _mm256_add_pd(vsum, v);
    }

    vsum = _mm256_hadd_pd(vsum, vsum);
    __m256d vsump = _mm256_permute2f128_pd(vsum, vsum, 0x1);
    vsum = _mm256_add_pd(vsum, vsump);

    return _mm256_cvtsd_f64(vsum);
}

struct mat
{
    float* data;
    const size_t sz;

    bool operator==(const mat& rhs) const
    {
        bool b_ret = true;
        const float tolerance = 0.1f;

        for (int i = 0; i < sz; i++) {
            for (int j = 0; j < sz; j++) {
                const float abs_diff = std::abs(this->data[i * sz + j] - rhs.data[i * sz + j]);
                b_ret = b_ret && (abs_diff < tolerance);
            }
        }

        return b_ret;
    }
};


struct mat_d
{
    double* data;
    const size_t sz_d;

    bool operator==(const mat& rhs) const
    {
        bool b_ret = true;
        const float tolerance = 0.1f;

        for (int i = 0; i < sz_d; i++) {
            for (int j = 0; j < sz_d; j++) {
                const float abs_diff = std::abs(this->data[i * sz_d + j] - rhs.data[i * sz_d + j]);
                b_ret = b_ret && (abs_diff < tolerance);
            }
        }

        return b_ret;
    }
};

void matmul(mat& mres, const mat& m1, const mat& m2)
{
    for (int i = 0; i < mres.sz; i++) {
        for (int j = 0; j < mres.sz; j++) {
            mres.data[i * mres.sz + j] = 0;
            for (int k = 0; k < mres.sz; k++) {
                mres.data[i * mres.sz + j] += m1.data[i * mres.sz + k] * m2.data[k * mres.sz + j];
            }
        }
    }    
}

void matmul_d(mat_d& mres, const mat_d& m1, const mat_d& m2)
{
    for (int i = 0; i < mres.sz_d; i++) {
        for (int j = 0; j < mres.sz_d; j++) {
            mres.data[i * mres.sz_d + j] = 0;
            for (int k = 0; k < mres.sz_d; k++) {
                mres.data[i * mres.sz_d + j] += m1.data[i * mres.sz_d + k] * m2.data[k * mres.sz_d + j];
            }
        }
    }
}

void transpose(mat& mres, const mat& m_out, const mat& m_in) {
    for (int i = 0; i < mres.sz; i++) {
        for (int j = 0; j < mres.sz; j++) {
            mres.data[j * mres.sz + i] = m_in.data[i * mres.sz + j];

        }
    }
}

void transpose_d(mat_d& mres, const mat_d& m_out, const mat_d& m_in) {
    for (int i = 0; i < mres.sz_d; i++) {
        for (int j = 0; j < mres.sz_d; j++) {
            mres.data[j * mres.sz_d + i] = m_in.data[i * mres.sz_d + j];

        }
    }
}


void matmul_simd(mat& mres, const mat& m1, const mat& m2, mat&transpose_mres) {

    // simd matrix multiplication single precision implamentation
    for (int i = 0; i < mres.sz; i++) {
        for (int j = 0; j < mres.sz; j++) {
            __m128 vsum = _mm_set1_ps(0.0f);

            const auto step = sizeof(__m128) / sizeof(mres.data[0]);
            for (std::size_t k = 0; k < mres.sz; k += step) {

                __m128 b = _mm_load_ps(&m1.data[i * mres.sz + k]);

                transpose(transpose_mres, m2, m2);
                __m128 c = _mm_load_ps(&transpose_mres.data[j * mres.sz + k]);


                vsum += _mm_mul_ps(b, c);


            }



            vsum = _mm_hadd_ps(vsum, vsum);
            vsum = _mm_hadd_ps(vsum, vsum);

            mres.data[i * mres.sz + j] = _mm_cvtss_f32(vsum);
        }
    }

    // pthread implamentation (not working)
    /*const unsigned num_threads = 4;
  
    ScePthread thread[num_threads];

    using tup_t = decltype(std::make_tuple(&mres.data[0], &m1.data[0], &m2.data[0], &transpose_mres.data[0], &mres, &m1, &m2, &transpose_mres, mres.sz));
    
    tup_t ptrs[num_threads];

    auto tproc = [](void* arg) -> void* {
        auto& [mresdata, m1data, m2data, transpose_mresdata, mres, m1, m2, tranpose_mres, sz] = *static_cast<tup_t*>(arg);
       
        for (int i = 0; i < sz; i++) {
            for (int j = 0; j < sz; j++) {
                __m128 vsum = _mm_set1_ps(0.0f);

                const auto step = sizeof(__m128) / sizeof(mres[0]);
                for (std::size_t k = 0; k < sz; k += step) {

                    __m128 b = _mm_load_ps(&m1data[i * sz + k]);

                    transpose_mresdata = transpose(transpose_mres, m2, m2);
                    
                    __m128 c = _mm_load_ps(&transpose_mresdata[j * sz + k]);


                    vsum += _mm_mul_ps(b, c);

                }



                vsum = _mm_hadd_ps(vsum, vsum);
                vsum = _mm_hadd_ps(vsum, vsum);

                mresdata[i * sz + j] = _mm_cvtss_f32(vsum);
            }
        }
        return mresdata;
    };

    const auto chunk = mres.sz / num_threads;
    for (unsigned i = 0; i < num_threads; i++) {
        ptrs[i] = std::make_tuple(&mres.data[i*chunk],&m1.data[i*chunk], &m2.data[i*chunk], &transpose_mres.data[i*chunk], &mres, &m1, &m2, &transpose_mres, chunk);
        scePthreadCreate(&thread[i], nullptr, tproc, &ptrs[i], nullptr);
        //pthread_create(&thread[i], nullptr, tproc, &ptrs[i]);
    }

    for (const auto& t : thread) { scePthreadJoin(t, nullptr); }*/

}


void matmul_simd_double(mat_d& mres, const mat_d& m1, const mat_d& m2, mat_d&transpose_mres_d) {

    //simd matrix multiplication double precision
    for (int i = 0; i < mres.sz_d; i++) {
        for (int j = 0; j < mres.sz_d; j++) {
            __m256d vsum = _mm256_set1_pd(0);

            const auto step = sizeof(__m256d) / sizeof(mres.data[0]);
            for (std::size_t k = 0; k < mres.sz_d; k += step) {

                __m256d b = _mm256_load_pd(&m1.data[i * mres.sz_d + k]);

                transpose_d(transpose_mres_d, m2, m2);
                __m256d c = _mm256_load_pd(&transpose_mres_d.data[j * mres.sz_d + k]);

                vsum += _mm256_mul_pd(b, c);
            }



            vsum = _mm256_hadd_pd(vsum, vsum);

            __m256d vsump = _mm256_permute2f128_pd(vsum, vsum, 0x1);

            vsum = _mm256_add_pd(vsum, vsump);

            mres.data[i * mres.sz_d + j] = _mm256_cvtsd_f64(vsum);
        }
    }

}


void print_mat(const mat& m) {
    for (int i = 0; i < m.sz; i++) {
        for (int j = 0; j < m.sz; j++) {
            std::cout << std::setw(3) << m.data[i * m.sz + j] << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}


void print_mat_d(const mat_d& m) {
    for (int i = 0; i < m.sz_d; i++) {
        for (int j = 0; j < m.sz_d; j++) {
            std::cout << std::setw(3) << m.data[i * m.sz_d + j] << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

// A simple initialisation pattern. For a 4x4 matrix:

// 1   2  3  4
// 5   6  7  8
// 9  10 11 12
// 13 14 15 16

void init_mat(mat& m) {
    int count = 1;
    for (int i = 0; i < m.sz; i++) {
        for (int j = 0; j < m.sz; j++) {
            m.data[i * m.sz + j] = count++;
        }
    }
}

void init_mat_d(mat_d& m) {
    int count = 1;
    for (int i = 0; i < m.sz_d; i++) {
        for (int j = 0; j < m.sz_d; j++) {
            m.data[i * m.sz_d + j] = count++;
        }
    }
}

// Creates an identity matrix. For a 4x4 matrix:

// 1 0 0 0
// 0 1 0 0
// 0 0 1 0
// 0 0 0 1

void identity_mat(mat& m) {
    int count = 0;
    for (int i = 0; i < m.sz; i++) {
        for (int j = 0; j < m.sz; j++) {
            m.data[i * m.sz + j] = (count++ % (m.sz + 1)) ? 0 : 1;
        }
    }
}

void identity_mat_d(mat_d& m) {
    int count = 0;
    for (int i = 0; i < m.sz_d; i++) {
        for (int j = 0; j < m.sz_d; j++) {
            m.data[i * m.sz_d + j] = (count++ % (m.sz_d + 1)) ? 0 : 1;
        }
    }
}

int main(int argc, char* argv[])
{
    
    
    // n.b. these calls to new have no alignment specifications
    //6 runs of 8x8 matrix multiplication
    for (int i = 0; i < 6; i++)
    {
        mat mres{ new (std::align_val_t(sizeof(__m128))) float[SZ * SZ],SZ }, m{ new (std::align_val_t(sizeof(__m128))) float[SZ * SZ],SZ }, id{ new (std::align_val_t(sizeof(__m128))) float[SZ * SZ],SZ };
        mat mres_simd{ new (std::align_val_t(sizeof(__m128))) float[SZ * SZ],SZ };
        mat transpose_mres{ new (std::align_val_t(sizeof(__m128))) float[SZ * SZ],SZ };
        mat_d mres_d{ new(std::align_val_t(sizeof(__m256d))) double[SZ * SZ],SZ }, m_d{ new (std::align_val_t(sizeof(__m256d))) double[SZ * SZ],SZ }, id_d{ new (std::align_val_t(sizeof(__m256))) double[SZ * SZ],SZ };
        mat_d mres_simd_double{ new(std::align_val_t(sizeof(__m256d))) double[SZ * SZ],SZ };
        mat_d transpose_mres_d{ new (std::align_val_t(sizeof(__m256d))) double[SZ * SZ],SZ };




        using namespace std::chrono;
        using tp_t = time_point<high_resolution_clock>;
        tp_t t1, t2;



        std::cout << "Each " << SZ << 'x' << SZ;
        std::cout << " matrix is " << sizeof(float) * SZ * SZ << " bytes.\n";

        init_mat(m);
        init_mat_d(m_d);
        identity_mat(id);
        identity_mat_d(id_d);


        matmul(mres, m, m);


        t1 = high_resolution_clock::now();
        matmul_simd(mres_simd, m, m, transpose_mres);
        t2 = high_resolution_clock::now();


        auto d_simd = duration_cast<microseconds>(t2 - t1).count();
        std::cout << d_simd << ' ' << "microseconds.\n";
        std::cout << mres.sz << ' ' << "simd single precision mres.sz.\n";


        matmul_d(mres_d, m_d, m_d);


        t1 = high_resolution_clock::now();
        matmul_simd_double(mres_simd_double, m_d, m_d, transpose_mres_d);
        t2 = high_resolution_clock::now();


        auto d_simd_d = duration_cast<microseconds>(t2 - t1).count();
        std::cout << d_simd_d << ' ' << "microseconds.\n";
        std::cout << mres.sz << ' ' << "simd double precision mres.sz.\n";


        print_mat(m);
        print_mat(m);
        print_mat(mres);
        print_mat(mres_simd);
        print_mat_d(mres_d);
        print_mat_d(mres_simd_double);


        //mres.operator==(mres_simd);




        delete[] mres.data;
        delete[] mres_simd.data;
        delete[] m.data;
        delete[] id.data;

        //std::cout << i << ' ' << "runs\n";
    }

    // 6 runs of 1024x1024 matrix multiplication
   /* for (int i = 0; i < 6; i++) {
        mat mres{ new (std::align_val_t(sizeof(__m128))) float[SZ2 * SZ2],SZ2 }, m{ new (std::align_val_t(sizeof(__m128))) float[SZ2 * SZ2],SZ2 }, id{ new (std::align_val_t(sizeof(__m128))) float[SZ2 * SZ2],SZ2 };
        mat mres_simd{ new (std::align_val_t(sizeof(__m128))) float[SZ2 * SZ2],SZ2 };
        mat transpose_mres{ new (std::align_val_t(sizeof(__m128))) float[SZ2 * SZ2],SZ2 };
        mat_d mres_d{ new(std::align_val_t(sizeof(__m256d))) double[SZ2 * SZ2],SZ2 }, m_d{ new (std::align_val_t(sizeof(__m256d))) double[SZ2 * SZ2],SZ2 }, id_d{ new (std::align_val_t(sizeof(__m256))) double[SZ2 * SZ2],SZ2 };
        mat_d mres_simd_double{ new(std::align_val_t(sizeof(__m256d))) double[SZ2 * SZ2],SZ2 };
        mat_d transpose_mres_d{ new (std::align_val_t(sizeof(__m256d))) double[SZ2 * SZ2],SZ2 };





        using namespace std::chrono;
        using tp_t = time_point<high_resolution_clock>;
        tp_t t1, t2;



        std::cout << "Each " << SZ2 << 'x' << SZ2;
        std::cout << " matrix is " << sizeof(float) * SZ2 * SZ2 << " bytes.\n";

        init_mat(m);
        init_mat_d(m_d);
        identity_mat(id);
        identity_mat_d(id_d);


        matmul(mres, m, m);


        t1 = high_resolution_clock::now();
        matmul_simd(mres_simd, m, m, transpose_mres);
        t2 = high_resolution_clock::now();


        auto d_simd = duration_cast<microseconds>(t2 - t1).count();
        std::cout << d_simd << ' ' << "microseconds.\n";
        std::cout << mres.sz << ' ' << "simd single precision mres.sz.\n";


        matmul_d(mres_d, m_d, m_d);


        t1 = high_resolution_clock::now();
        matmul_simd_double(mres_simd_double, m_d, m_d, transpose_mres_d);
        t2 = high_resolution_clock::now();


        auto d_simd_d = duration_cast<microseconds>(t2 - t1).count();
        std::cout << d_simd_d << ' ' << "microseconds.\n";
        std::cout << mres.sz << ' ' << "simd double precision mres.sz.\n";

        print_mat(m);
        print_mat(m);
        print_mat(mres);
        print_mat(mres_simd);
        print_mat_d(mres_d);
        print_mat_d(mres_simd_double);


        //mres.operator==(mres_simd);




        delete[] mres.data;
        delete[] mres_simd.data;
        delete[] m.data;
        delete[] id.data;

        std::cout << i << ' ' << "runs\n";
    }*/

    /*const bool correct = mres_simd == mres;
    assert(correct); // uncomment when you have implemented matmul_simd
    return correct ? 0 : -1;*/
}

