// VMDモーションを平滑化する

//#include <fstream>
//#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <Eigen/Core>
#include <unsupported/Eigen/FFT>
#include "VMD.h"
#include "MMDFileIOUtil.h"
#include "smoothvmd.h"

using namespace Eigen;
using namespace MMDFileIOUtil;
using namespace std;

void interpolate_frame(vector<VMD_Frame>& fv)
{
  // not implemented yet
}

void interpolate_morph(vector<VMD_Morph>& fv)
{
  // not implemented yet
}

void lowpass_filter(vector<float>& v, float cutoff_freq)
{
  //  cout << "lowpass_filter" << endl;
  FFT<float> fft;
  vector<complex<float>> freqvec;
  fft.fwd(freqvec, v);

  // フィルタリング
  //  const float cutoff_freq = 5.0; // カットオフ周波数[Hz]
  const float sampling_freq = 30.0; // サンプリング周波数[Hz]。MMDは30FPSなので。
  int data_size = v.size();
  int cutoff_idx;
  // cutoff_idx / data_size = cutoff_freq / sampling_freq
  cutoff_idx = cutoff_freq * data_size / sampling_freq;
  //  cout << "cutoff_idx: " << cutoff_idx << endl;
  
  vector<complex<float>> filtered(data_size);
  for (int i = 0; i < data_size; i++) {
    if (i < cutoff_idx) {
      filtered[i] = freqvec[i];
    } else {
      filtered[i] = 0.0;
    }
  }
  fft.inv(v, filtered);
}

void smooth_bone_frame(vector<VMD_Frame>& fv, float cutoff_freq)
{
  sort(fv.begin(), fv.end());
  // uint32_t first_frame_num = fv.front().number;
  // uint32_t last_frame_num = fv.back().number;
  interpolate_frame(fv); // キーフレームの隙間をなくす
  // ローパスフィルタにかける
  vector<float> x;
  for_each(fv.begin(), fv.end(), [&x](VMD_Frame f) { x.push_back(f.position.x()); });
  lowpass_filter(x, cutoff_freq);
  for (unsigned int i = 0; i < x.size(); i++) {
    fv[i].position.x() = x[i];
  }
  vector<float> y;
  for_each(fv.begin(), fv.end(), [&y](VMD_Frame f) { y.push_back(f.position.y()); });
  lowpass_filter(y, cutoff_freq);
  for (unsigned int i = 0; i < y.size(); i++) {
    fv[i].position.y() = y[i];
  }
  vector<float> z;
  for_each(fv.begin(), fv.end(), [&z](VMD_Frame f) { z.push_back(f.position.y()); });
  lowpass_filter(z, cutoff_freq);
  for (unsigned int i = 0; i < z.size(); i++) {
    fv[i].position.z() = z[i];
  }
}

void smooth_morph_frame(vector<VMD_Morph>& mv, float cutoff_freq)
{
  sort(mv.begin(), mv.end());
  // uint32_t first_frame_num = mv.front().frame;
  // uint32_t last_frame_num = mv.back().frame;
  interpolate_morph(mv); // キーフレームの隙間をなくす
  // ローパスフィルタにかける
  vector<float> w;
  for_each(mv.begin(), mv.end(), [&w](VMD_Morph s) { w.push_back(s.weight); });
  lowpass_filter(w, cutoff_freq);
  for (unsigned int i = 0; i < w.size(); i++) {
    //    cout << "weight: " << mv[i].weight << " => " << w[i] << endl;
    mv[i].weight = w[i];
    if (w[i] > 1.0) {
      mv[i].weight = 1.0;
    } else if (w[i] < 0.0) {
      mv[i].weight = 0.0;
    }
  }
}

