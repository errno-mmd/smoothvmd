#include <fstream>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "VMD.h"
#include "smooth_reduce.h"

using namespace std;
namespace opt = boost::program_options;

void usage(char *prog, opt::options_description desc)
{
  cout << "usage: " << prog << " [options] input.vmd output.vmd" << endl;
  cout << desc << endl;
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
