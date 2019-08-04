#include <Eigen/Core>
#include <Eigen/Geometry>
#include <cmath>
#include <iostream>
#include <random>
#include "gtest/gtest.h"
#include "interpolate.h"
#include "VMD.h"

VMD_Frame recreate_bezier_parameter(VMD_Frame head, VMD_Frame tail)
{
  vector<VMD_Frame> fv = {head, tail};
  fv = fill_bone_frame(fv, true);
  optimize_bezier_parameter(tail, fv, head.number, tail.number);
  return tail;
}

TEST(InterpolationTest, BezierCurveFitting) {
  Eigen::Quaternionf q1(Eigen::AngleAxisf(3.14f/4, Eigen::Vector3f::UnitY()));
  VMD_Frame head = VMD_Frame("center", 10, Eigen::Vector3f(1, 2, 3), q1);
  Eigen::Quaternionf q2(Eigen::AngleAxisf(3.14f/4, Eigen::Vector3f::UnitX()));
  VMD_Frame tail = VMD_Frame("center", 20, Eigen::Vector3f(2, -2, 2), q2);
  VMD_Frame f;

  tail.set_interpolation_x(10, 100, 110, 30);
  f = recreate_bezier_parameter(head, tail);
  for (int i = 0; i < 64; i += 4) {
    EXPECT_NEAR(f.interpolation[i], tail.interpolation[i], 5) << "i = " << i;
  }

  tail.set_interpolation_r(20, 5, 90, 127);
  f = recreate_bezier_parameter(head, tail);
  for (int i = 0; i < 64; i += 4) {
    EXPECT_NEAR(f.interpolation[i], tail.interpolation[i], 5) << "i = " << i;
  }

}
