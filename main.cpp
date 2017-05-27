#include <chrono>
#include <atomic>
#include <math.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <cmath>
#include <fstream>

using namespace std;

//realization of clock
inline chrono::high_resolution_clock::time_point get_current_time_fenced()
{
    atomic_thread_fence(memory_order_seq_cst);
    auto res_time = chrono::high_resolution_clock::now();
    atomic_thread_fence(memory_order_seq_cst);
    return res_time;
}
template<class D>
inline long long to_us(const D& d)
{
    return chrono::duration_cast<chrono::microseconds>(d).count();
}



double sum_calc(int m, double x1, double x2) {
    double sum1 = 0;
    double sum2 = 0;
    double result_sum;
    for (int i = 1; i <= m; ++i)
    {
        sum1 += i * cos((i + 1) * x1 + 1);
        sum2 += i * cos((i + 1) * x2 + 1);
    }
    result_sum = - sum1 * sum2;
    return result_sum;
}



double integral(double x1, double x2, double y1, double y2, int m, double pr) {
    double sum = 0;
    for (double i = x1; i <= x2; i += pr) {
        for (double j = y1; j <= y2; j += pr) {
            sum += sum_calc(m, i + pr / 2.0, j + pr / 2.0) * pr * pr;
        }
    }
    return sum;
}
mutex mx;
void thread_integral(double x1, double x2, double y1, double y2, int m, double pr, double* r) {
    auto result = integral(x1, x2, y1, y2, m, pr);
    lock_guard<mutex> integ(mx);
    *r += result;
}




int main(){

    //    double abs_er = 0.3;
    //    double rel_er = 0.3;
    //    int num_of_threads = 30;
    //    double x1 = 0;
    //    double x2 = 3;
    //    double y1 = 0;
    //    double y2 = 4;
    //    int m=6;

    double abs_er, rel_er, x2, x1, y2, y1;
    int m, num_of_threads;
    cout << "Please enter absolute error: ";
    cin >> abs_er;
    cout << "Please enter relative error: ";
    cin >> rel_er;
    cout << "Please enter X1: ";
    cin >> x1;
    cout << "Please enter X2: ";
    cin >> x2;
    cout << "Please enter Y1: ";
    cin >> y1;
    cout << "Please enter Y2: ";
    cin >> y2;
    cout << "Please m: ";
    cin >> m;
    cout << "Please enter number of threads: ";
    cin >> num_of_threads;


    double pr = 1E-3;

    double thread_integ = 0;

    double step_x = (x2 - x1) / num_of_threads;

    thread threads[num_of_threads];


    //   Integral without threads
    auto t_1 = get_current_time_fenced();
    double integ = integral(x1, x2, y1, y2, m, pr);
    auto t_2 = get_current_time_fenced();


    //   Integral with threads
    auto t1 = get_current_time_fenced();
    for (int i = 0; i < num_of_threads; ++i) {
        threads[i] = thread(thread_integral, x1, x1 + step_x, y1, y2, m, pr, &thread_integ);
        x1 += step_x;
    }

    for (int j = 0; j < num_of_threads; ++j) {
        threads[j].join();
    }

    auto t2 = get_current_time_fenced();

    cout << "\n----------------------\n"<<endl;
    cout << "Integral without threads = " << integ << endl;
    cout << "Threads integral = " << thread_integ << endl;
    cout << "Time without threads = " << to_us(t_2 - t_1)/ (double)(1000) << " ms" << endl;
    cout << "Time with threads = " << to_us(t2 - t1)/ (double)(1000) << " ms" << endl;


    // Absolute and relative errors

    double absol = abs(integ - thread_integ);
    double relat = abs((integ - thread_integ) / max(integ,thread_integ));

    if (absol <= abs_er){
        cout << "Absolute error " << absol<<" is okay\n";}
    else{
        cout << "Absolute error" << absol<<" is not okay\n";}

    if (relat <= rel_er){
        cout << "Relative error " << relat<<" is okay\n";}
    else{
        cout << "Relative error " << relat<<" is not okay\n";}

    cout << "\n---------------------\n"<<endl;
    string file;
    cout << "Please enter name of file to write result: ";
    cin >> file;
    ofstream write_file;
    write_file.open(file);
    write_file << "Integral without threads = " << integ << endl;
    write_file << "Threads integral = " << thread_integ << endl;
    write_file << "Absolute error = " << absol << endl;
    write_file << "Relative error = " << relat << endl;
    write_file <<"Time without threads = " << to_us(t_2 - t_1)/(double)(1000) << " ms" << endl;
    write_file <<"Time with threads = " << to_us(t2 - t1)/ (double)(1000) << " ms" << endl;

    cout << "\nSuccess"<<endl;
    return 0;
}
