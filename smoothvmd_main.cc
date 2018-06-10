#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <Eigen/Core>
#include <unsupported/Eigen/FFT>
#include <boost/program_options.hpp>
#include "VMD.h"
#include "MMDFileIOUtil.h"
#include "smoothvmd.h"
#include "reducevmd.h"

using namespace Eigen;
using namespace MMDFileIOUtil;
using namespace std;
namespace opt = boost::program_options;

void usage(char *prog, opt::options_description desc)
{
  cout << "usage: " << prog << " [options] input.vmd output.vmd" << endl;
  cout << desc << endl;
}

bool smooth_and_reduce(VMD& vmd, float cutoff_freq, float threshold_pos, float threshold_rot, float threshold_morph)
{
  // キーフレームをボーンごとに分ける
  map<string, vector<VMD_Frame>> frame_map;
  for (unsigned int i = 0; i < vmd.frame.size(); i++) {
    VMD_Frame frame = vmd.frame[i];
    string name;
    sjis_to_utf8(frame.bonename, name, frame.bonename_len);
    frame_map[name].push_back(frame);
  }

  // ボーンごとに平滑化と間引きを行う
  for (auto iter = frame_map.begin(); iter != frame_map.end(); iter++) {
    string name = iter->first;
    vector<VMD_Frame>& fv = iter->second;
    if (fv.size() > 0) {
      smooth_bone_frame(fv, cutoff_freq);
      VMD_Frame last_frame = fv.back();
      fv = reduce_bone_frame(fv, 0, fv.size() - 1, threshold_pos, threshold_rot);
      fv.push_back(last_frame);
    }
  }

  // ボーンキーフレームを入れ替える
  cout << "vmd.frame.size(original): " << vmd.frame.size() << endl;
  vmd.frame.clear();
  for (auto iter = frame_map.begin(); iter != frame_map.end(); iter++) {
    vector<VMD_Frame> fv = iter->second;
    for (unsigned int i = 0; i < fv.size(); i++) {
      vmd.frame.push_back(fv[i]);
    }
  }
  cout << "vmd.frame.size(reduced): " << vmd.frame.size() << endl;

  
  // キーフレームをモーフごとに分ける
  map<string, vector<VMD_Morph>> morph_map;
  for (unsigned int i = 0; i < vmd.morph.size(); i++) {
    VMD_Morph morph = vmd.morph[i];
    string name;
    sjis_to_utf8(morph.name, name, morph.name_len);
    morph_map[name].push_back(morph);
  }

  // モーフごとに平滑化と間引きを行う
  for (auto iter = morph_map.begin(); iter != morph_map.end(); iter++) {
    string name = iter->first;
    vector<VMD_Morph>& mv = iter->second;
    if (mv.size() > 0) {
      smooth_morph_frame(mv, cutoff_freq);
      VMD_Morph last_morph = mv.back();
      mv = reduce_morph_frame(mv, 0, mv.size() - 1, threshold_morph);
      mv.push_back(last_morph);
    }
  }

  // 表情キーフレームを入れ替える
  cout << "vmd.morph.size(original): " << vmd.morph.size() << endl;
  vmd.morph.clear();
  for (auto iter = morph_map.begin(); iter != morph_map.end(); iter++) {
    vector<VMD_Morph> sv = iter->second;
    for (unsigned int i = 0; i < sv.size(); i++) {
      vmd.morph.push_back(sv[i]);
    }
  }
  cout << "vmd.morph.size(reduced) : " << vmd.morph.size() << endl;
  
  return true;
}


int main(int argc, char *argv[])
{
  opt::options_description desc("options");
  desc.add_options()
    ("help", "help message")
    ("cutoff", opt::value<float>(), "cutoff frequency [Hz]")
    ("th_pos", opt::value<float>(), "threshold(position) for keyframe reduction")
    ("th_rot", opt::value<float>(), "threshold(rotation) for keyframe reduction [degree]")
    ("th_morph", opt::value<float>(), "threshold(morph) for keyframe reduction")
    ;

  opt::options_description hidden("hidden options");
  hidden.add_options()
    ("input-file", opt::value<string>(), "input VMD file")
    ("output-file", opt::value<string>(), "output VMD file")
    ;

  opt::options_description all_options;
  all_options.add(desc).add(hidden);

  opt::positional_options_description p;
  opt::variables_map vm;

  string fname_in;
  string fname_out;
  float cutoff_freq = 5.0; // [Hz]
  float threshold_pos = 0.5;
  float threshold_rot = 3.0; // [degree]
  float threshold_morph = 0.1; // 0～1

  try {
    p.add("input-file", 1);
    p.add("output-file", 1);
    opt::store(opt::command_line_parser(argc, argv).
	       options(all_options).positional(p).run(), vm);
    opt::notify(vm);

    if (vm.count("help") || !vm.count("input-file") || !vm.count("output-file")) {
      usage(argv[0], desc);
      return 1;
    }
    if (vm.count("cutoff")) {
      cutoff_freq = vm["cutoff"].as<float>();
    }
    if (vm.count("th_pos")) {
      threshold_pos = vm["th_pos"].as<float>();
    }
    if (vm.count("th_rot")) {
      threshold_rot = vm["th_rot"].as<float>();
    }
    if (vm.count("th_morph")) {
      threshold_morph = vm["th_morph"].as<float>();
    }
    fname_in = vm["input-file"].as<string>();
    fname_out = vm["output-file"].as<string>();
  } catch (exception& e) {
    cerr << e.what() << endl;
    return 1;
  }
  cout << "input file: " << fname_in << endl;
  cout << "output file: " << fname_out << endl;
  cout << "cutoff:" << cutoff_freq << endl;
  cout << "threshold(position): " << threshold_pos << endl;
  cout << "threshold(rotation): " << threshold_rot << endl;
  cout << "threshold(morph): " << threshold_morph << endl;
  
  ifstream in(fname_in, ios::binary);
  VMD v;
  v.input(in);
  in.close();

  bool success;
  success = smooth_and_reduce(v, cutoff_freq, threshold_pos, threshold_rot, threshold_morph);
  if (! success) {
    return 1;
  }

  ofstream out(fname_out, ios::binary);
  v.output(out);
  out.close();

  return 0;
}
