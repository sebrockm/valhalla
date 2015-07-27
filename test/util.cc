#include "test.h"
#include "valhalla/midgard/util.h"
#include "valhalla/midgard/distanceapproximator.h"

using namespace valhalla::midgard;

namespace {

void TestGetTurnDegree() {
  // Slight Right
  if (GetTurnDegree(315, 335) != 20)
    throw std::runtime_error("Invalid turn degree");
  // Right
  if (GetTurnDegree(0, 90) != 90)
    throw std::runtime_error("Invalid turn degree");
  // Right
  if (GetTurnDegree(90, 180) != 90)
    throw std::runtime_error("Invalid turn degree");
  // Sharp Right
  if (GetTurnDegree(180, 340) != 160)
    throw std::runtime_error("Invalid turn degree");
  // Sharp Right
  if (GetTurnDegree(180, 352) != 172)
    throw std::runtime_error("Invalid turn degree");
  // Sharp Left
  if (GetTurnDegree(180, 40) != 220)
    throw std::runtime_error("Invalid turn degree");
  // Sharp Left
  if (GetTurnDegree(180, 10) != 190)
    throw std::runtime_error("Invalid turn degree");
  // Left
  if (GetTurnDegree(0, 180) != 180)
    throw std::runtime_error("Invalid turn degree");
  // Left
  if (GetTurnDegree(270, 180) != 270)
    throw std::runtime_error("Invalid turn degree");
  // Slight Left
  if (GetTurnDegree(90, 70) != 340)
    throw std::runtime_error("Invalid turn degree");
  // Continue
  if (GetTurnDegree(358, 2) != 4)
    throw std::runtime_error("Invalid turn degree");
}

void TestGetTime() {
  if (GetTime(100, 100) != 3600)
    throw std::runtime_error("Invalid time");
  if (GetTime(5, 20) != 900)
    throw std::runtime_error("Invalid time");
}

void AppxEqual() {
  if(!equal<float>(-136.170790, -136.170800, .00002f))
    throw std::runtime_error("Should be equal");
  if(!equal<float>(-136.170800, -136.170790, .00002f))
    throw std::runtime_error("Should be equal");
  if(!equal<float>(16.645590, 16.645580, .00002f))
    throw std::runtime_error("Should be equal");
  if(!equal<float>(76.627980, 76.627970, .00002f))
    throw std::runtime_error("Should be equal");
  if(!equal<int>(0, 0))
    throw std::runtime_error("Should be equal");
  if(!equal<float>(1, 1, 0))
    throw std::runtime_error("Should be equal");
}

void MemoryStatus() {
  memory_status status({"VmSize", "VmSwap", "VmPeak"});

  //should have each of these
  for(const auto& key : {"VmSize", "VmSwap", "VmPeak"}) {
    auto value = status.metrics.find(key);
    if(value == status.metrics.end())
      throw std::runtime_error("Missing memory statistic for " + std::string(key));
    if(value->second.first < 0.)
      throw std::runtime_error("Negative memory usage values are not allowed");
    if(value->second.second.back() != 'B')
      throw std::runtime_error("Units should be some magnitude of bytes");
  }
}

void TestClamp() {
  if(!equal<float>(circular_range_clamp<float>(467, -90, 90),-73))
    throw std::runtime_error("Wrong clamp value");
  if(!equal<float>(circular_range_clamp<float>(-467, -90, 90), 73))
    throw std::runtime_error("Wrong clamp value");
  if(!equal<float>(circular_range_clamp<float>(7, -90, 90), 7))
    throw std::runtime_error("Wrong clamp value");
  if(!equal<float>(circular_range_clamp<float>(-67, -90, 90), -67))
    throw std::runtime_error("Wrong clamp value");
  if(!equal<float>(circular_range_clamp<float>(-97, -90, 90), 83))
    throw std::runtime_error("Wrong clamp value");
  if(!equal<float>(circular_range_clamp<float>(-97.2, -90, 90), 82.8))
    throw std::runtime_error("Wrong clamp value");
  if(!equal<float>(circular_range_clamp<float>(-180, -90, 90), 0))
    throw std::runtime_error("Wrong clamp value");
  if(!equal<float>(circular_range_clamp<float>(270, -90, 90), -90))
    throw std::runtime_error("Wrong clamp value");
  if(!equal<float>(circular_range_clamp<float>(369, 0, 360), 9))
    throw std::runtime_error("wrong clamp value");
  if(!equal<float>(circular_range_clamp<float>(-369, 0, 360), 351))
    throw std::runtime_error("wrong clamp value");
  if(!equal<float>(circular_range_clamp<float>(739, -45, -8), -38))
    throw std::runtime_error("wrong clamp value");
}


void TestResample() {
  //try it
  auto input_shape = decode<std::vector<PointLL>>("cfcglAlj_~pCsiAdOaeAvN}_@|ImTyBiW}I}TsQ}d@}^cUyWcGaHoNcPc`@oh@ykAw`BuTeZkt@emAquAk}BucAelBwXqg@o|@{~@oSiBuOkAiCtDw`AxMaHxBmUpGcFzAe_Atm@_w@ju@wb@hWu_@~Ied@}@wb@uO}_@uDmTrFwb@~g@wNfw@jGrxBiClJy\\yBsvC_hAwN|@wNl^qBvYiH`fAjGxwGn|@vpFiC|eD?z@cK|pCcAhWsGjj@qQju@iWtOwXrF}s@?m}Agw@g`BmhAycC{rB}gAgw@ceAiw@_|AofAcPyByHsQy[kUgh@qHyvDjV_cApGa{@vm@iWfYuD?{AhLwXn^m@hL_I|@slClcEwb@fNalAoH{mA}^kdBk`Aso@iLm^z@cBzAsK|JvD~|@vXv`Am@rQwDpHso@e[_xDu~Cy}GqvEoT{Ked@oHen@z@gTzKwX|^egBnbEuh@dmAwdCdrF_eC~uF");
  auto resampled = resample_spherical_polyline(input_shape, 100);

  //check that nothing is too far apart
  for(auto p = resampled.cbegin() + 1; p != resampled.cend(); ++p) {
    auto sqdist = DistanceApproximator::DistanceSquared(*p, *(p - 1));
    auto dist = 1.f / FastInvSqrt(sqdist);
    if(dist > 105)
      throw std::runtime_error("Distance between any two points on the resampled line cannot be further than resample distance");
  }

  //all the points better be within a meter or so of the original line
  for(const auto p : resampled) {
    auto cp = p.ClosestPoint(input_shape);
    auto dist = std::get<1>(cp);
    if(!equal(dist, 0.f, 1.2f)) {
      throw std::runtime_error("Sampled point was not found on original line");
    }
  }
}

}

int main() {
  test::suite suite("util");

  // GetTurnDegree
  suite.test(TEST_CASE(TestGetTurnDegree));

  // GetTime
  suite.test(TEST_CASE(TestGetTime));

  suite.test(TEST_CASE(AppxEqual));

  suite.test(TEST_CASE(MemoryStatus));

  suite.test(TEST_CASE(TestClamp));

  suite.test(TEST_CASE(TestResample));

  return suite.tear_down();
}
