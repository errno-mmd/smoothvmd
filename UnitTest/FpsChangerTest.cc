#include "gtest/gtest.h"
#include "fpschanger.h"
#include "interpolate.h"
#include "VMD.h"

class FpsChangerTest : public ::testing::Test {
public:
  virtual void SetUp() {
    Eigen::Quaternionf q1(Eigen::AngleAxisf(3.14f/4, Eigen::Vector3f::UnitY()));
    VMD_Frame head = VMD_Frame("center", 0, Eigen::Vector3f(2, 3, 4), q1);
    Eigen::Quaternionf q2(Eigen::AngleAxisf(3.14f/4, Eigen::Vector3f::UnitZ()));
    VMD_Frame tail = VMD_Frame("center", LAST_FRAME, Eigen::Vector3f(-1, 1, 2), q2);  

    v0.push_back(head);
    for (unsigned int i = 1; i < tail.number; i++) {
      VMD_Frame f = interpolate_frame(head, tail, i, false);
      v0.push_back(f);
    }
    v0.push_back(tail);

    VMD_Morph head_m = VMD_Morph();
    head_m.frame = 0;
    head_m.weight = 0.98;
    VMD_Morph tail_m = VMD_Morph();
    tail_m.frame = LAST_FRAME;
    tail_m.weight = 0.01;
    m0.push_back(head_m);
    for (unsigned int i = 1; i < tail_m.frame; i++) {
      VMD_Morph m = interpolate_morph(head_m, tail_m, i);
      m0.push_back(m);
    }
    m0.push_back(tail_m);
  }

  // virtual void TearDown() {}

  const unsigned int LAST_FRAME = 110;
  vector<VMD_Frame> v0;
  vector<VMD_Morph> m0;
};

TEST_F(FpsChangerTest, Bone_Up_Down) {
  vector<VMD_Frame> v1 = change_fps_bone(v0, 24.0, 30.0, true);
  EXPECT_EQ((v0.size() - 1) * 30 / 24 + 1, v1.size());
  vector<VMD_Frame> v2 = change_fps_bone(v1, 30.0, 24.0, true);
  EXPECT_EQ((v1.size() - 1) * 24 / 30 + 1, v2.size());
  for (unsigned int i = 0; i < v2.size(); i++) {
    EXPECT_NEAR(v0[i].position.x(), v2[i].position.x(), 1.0e-6);
    EXPECT_NEAR(v0[i].position.y(), v2[i].position.y(), 1.0e-6);
    EXPECT_NEAR(v0[i].position.z(), v2[i].position.z(), 1.0e-6);
    EXPECT_NEAR(v0[i].rotation.w(), v2[i].rotation.w(), 1.0e-6);
    EXPECT_NEAR(v0[i].rotation.x(), v2[i].rotation.x(), 1.0e-6);
    EXPECT_NEAR(v0[i].rotation.y(), v2[i].rotation.y(), 1.0e-6);
    EXPECT_NEAR(v0[i].rotation.z(), v2[i].rotation.z(), 1.0e-6);
  }

  vector<VMD_Frame> v3 = change_fps_bone(v0, 7.0, 25.0, true);
  EXPECT_EQ((v0.size() - 1) * 25 / 7 + 1, v3.size());
  vector<VMD_Frame> v4 = change_fps_bone(v3, 25.0, 7.0, true);
  EXPECT_EQ((v3.size() - 1) * 7 / 25 + 1, v4.size());
  for (unsigned int i = 0; i < v4.size(); i++) {
    EXPECT_NEAR(v0[i].position.x(), v4[i].position.x(), 1.0e-6);
    EXPECT_NEAR(v0[i].position.y(), v4[i].position.y(), 1.0e-6);
    EXPECT_NEAR(v0[i].position.z(), v4[i].position.z(), 1.0e-6);
    EXPECT_NEAR(v0[i].rotation.w(), v4[i].rotation.w(), 1.0e-6);
    EXPECT_NEAR(v0[i].rotation.x(), v4[i].rotation.x(), 1.0e-6);
    EXPECT_NEAR(v0[i].rotation.y(), v4[i].rotation.y(), 1.0e-6);
    EXPECT_NEAR(v0[i].rotation.z(), v4[i].rotation.z(), 1.0e-6);
  }
}

TEST_F(FpsChangerTest, Bone_Down_Up) {
  vector<VMD_Frame> v1 = change_fps_bone(v0, 51.0, 25.0, true);
  EXPECT_EQ((v0.size() - 1) * 25 / 51 + 1, v1.size());
  vector<VMD_Frame> v2 = change_fps_bone(v1, 25.0, 51.0, true);
  EXPECT_EQ((v1.size() - 1) * 51 / 25 + 1, v2.size());
  for (unsigned int i = 0; i < v2.size(); i++) {
    EXPECT_NEAR(v0[i].position.x(), v2[i].position.x(), 1.0e-6);
    EXPECT_NEAR(v0[i].position.y(), v2[i].position.y(), 1.0e-6);
    EXPECT_NEAR(v0[i].position.z(), v2[i].position.z(), 1.0e-6);
    EXPECT_NEAR(v0[i].rotation.w(), v2[i].rotation.w(), 1.0e-6);
    EXPECT_NEAR(v0[i].rotation.x(), v2[i].rotation.x(), 1.0e-6);
    EXPECT_NEAR(v0[i].rotation.y(), v2[i].rotation.y(), 1.0e-6);
    EXPECT_NEAR(v0[i].rotation.z(), v2[i].rotation.z(), 1.0e-6);
  }
}

TEST_F(FpsChangerTest, Morph_Up_Down) {
  vector<VMD_Morph> m1 = change_fps_morph(m0, 25.0, 60.0);
  EXPECT_EQ((m0.size() - 1) * 60 / 25 + 1, m1.size());
  vector<VMD_Morph> m2 = change_fps_morph(m1, 60.0, 25.0);
  EXPECT_EQ((m1.size() - 1) * 25 / 60 + 1, m2.size());
  for (unsigned int i = 0; i < m2.size(); i++) {
    EXPECT_NEAR(m0[i].weight, m2[i].weight, 1.0e-6);
  }
}

TEST_F(FpsChangerTest, Morph_Down_Up) {
  vector<VMD_Morph> m1 = change_fps_morph(m0, 24.0, 15.0);
  EXPECT_EQ((m0.size() - 1) * 15 / 24 + 1, m1.size());
  vector<VMD_Morph> m2 = change_fps_morph(m1, 15.0, 24.0);
  EXPECT_EQ((m1.size() - 1) * 24 / 15 + 1, m2.size());
  for (unsigned int i = 0; i < m2.size(); i++) {
    EXPECT_NEAR(m0[i].weight, m2[i].weight, 1.0e-6);
  }
}
