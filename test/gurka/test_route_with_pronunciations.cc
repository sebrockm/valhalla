#include "gurka.h"
#include <gtest/gtest.h>

#if !defined(VALHALLA_SOURCE_DIR)
#define VALHALLA_SOURCE_DIR
#endif

using namespace valhalla;

const std::unordered_map<std::string, std::string> build_config{
    {"mjolnir.admin", {VALHALLA_SOURCE_DIR "test/data/netherlands_admin.sqlite"}},
    {"odin.markup_formatter.markup_enabled", "true"},
    {"odin.markup_formatter.phoneme_format",
     "%1% (<span class=<QUOTES>phoneme<QUOTES>>/%2%/</span>)"},
};

class RouteWithExitSignPronunciation : public ::testing::Test {
protected:
  static gurka::map map;

  static void SetUpTestSuite() {
    constexpr double gridsize_metres = 100;

    const std::string ascii_map = R"(
                       J
                       |
                       |
                       |
                       I
                      /|\
                    /  |  \
                  /    |    \
           L----K-------------H----G
           A----B-------------E----F
                  \    |    /
                    \  |  /
                      \|/
                       C
                       |
                       |
                       |
                       D


    )";

    const gurka::ways ways =
        {{"ABEF", {{"highway", "motorway"}, {"name", ""}, {"ref", "I 70"}, {"oneway", "yes"}}},
         {"GHKL", {{"highway", "motorway"}, {"name", ""}, {"ref", "I 70"}, {"oneway", "yes"}}},
         {"JICD",
          {{"highway", "primary"},
           {"name", "Lancaster Road"},
           {"name:pronunciation", "ˈlæŋkəstər ˈɹoʊd"},
           {"ref", "SR 37"},
           {"ref:pronunciation", "ˈsinjər 37"}}},
         {"BC",
          {{"highway", "motorway_link"},
           {"name", ""},
           {"oneway", "yes"},
           {"junction:ref", "126B"},
           {"junction:ref:pronunciation", "1 26bi"},
           {"destination", "Granville;Lancaster"},
           {"destination:pronunciation", "ˈgɹænvɪl;ˈlæŋkəstər"},
           {"destination:street", "Lancaster Road"},
           {"destination:street:pronunciation", "ˈlæŋkəstər ˈɹoʊd"},
           {"destination:ref", "SR 37"},
           {"destination:ref:pronunciation", "ˈsinjər 37"}}},
         {"CE",
          {{"highway", "motorway_link"},
           {"name", ""},
           {"oneway", "yes"},
           {"destination:ref", "I 70 East"}}},
         {"HI",
          {{"highway", "motorway_link"},
           {"name", ""},
           {"oneway", "yes"},
           {"junction:ref", "126B"},
           {"junction:ref:pronunciation", "1 26bi"},
           {"destination:street:to", "Main Street"},
           {"destination:street:to:pronunciation", "meɪn strit"},
           {"destination:ref:to", "I 80"},
           {"destination:ref:to:pronunciation", "aɪ 80"}}},
         {"IK",
          {{"highway", "motorway_link"},
           {"name", ""},
           {"oneway", "yes"},
           {"destination:ref", "I 70 West"}}}};

    const auto layout =
        gurka::detail::map_to_coordinates(ascii_map, gridsize_metres, {5.1079374, 52.0887174});
    map = gurka::buildtiles(layout, ways, {}, {},
                            "test/data/gurka_route_with_exit_sign_pronunciation", build_config);
  }
};

gurka::map RouteWithExitSignPronunciation::map = {};

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithExitSignPronunciation, CheckStreetNamesAndSigns1) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"A", "D"}, "auto");
  gurka::assert::raw::expect_path(result, {"I 70", "", "Lancaster Road/SR 37"});

  // Verify starting on I 70
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 1);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "I 70");

  // Verify sign pronunciations - alphabet & value
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets_size(),
            2);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(0)
                .text(),
            "SR 37");

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(1)
                .text(),
            "Lancaster Road");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(1)
                .pronunciation()
                .alphabet(),
            Pronunciation_Alphabet_kIpa);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(1)
                .pronunciation()
                .value(),
            "ˈlæŋkəstər ˈɹoʊd");

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations_size(),
            2);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .text(),
            "Granville");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .pronunciation()
                .alphabet(),
            Pronunciation_Alphabet_kIpa);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .pronunciation()
                .value(),
            "ˈgɹænvɪl");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .text(),
            "Lancaster");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .pronunciation()
                .alphabet(),
            Pronunciation_Alphabet_kIpa);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .pronunciation()
                .value(),
            "ˈlæŋkəstər");

  // Verify sign pronunciation instructions
  gurka::assert::raw::expect_instructions_at_maneuver_index(
      result, maneuver_index, "Take exit 126B onto SR 37/Lancaster Road toward Granville/Lancaster.",
      "", "Take exit 126B (<span class=&quot;phoneme&quot;>/1 26bi/</span>).",
      "Take exit 126B (<span class=&quot;phoneme&quot;>/1 26bi/</span>) onto SR 37 (<span class=&quot;phoneme&quot;>/ˈsinjər 37/</span>), Lancaster Road (<span class=&quot;phoneme&quot;>/ˈlæŋkəstər ˈɹoʊd/</span>) toward Granville (<span class=&quot;phoneme&quot;>/ˈgɹænvɪl/</span>), Lancaster (<span class=&quot;phoneme&quot;>/ˈlæŋkəstər/</span>).",
      "");

  // Verify street name pronunciation - alphabet & value
  ++maneuver_index;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 2);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "Lancaster Road");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(0)
                .pronunciation()
                .alphabet(),
            Pronunciation_Alphabet_kIpa);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(0)
                .pronunciation()
                .value(),
            "ˈlæŋkəstər ˈɹoʊd");
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "SR 37");

  // Verify street name pronunciation instructions
  gurka::assert::raw::expect_instructions_at_maneuver_index(
      result, maneuver_index, "Turn right onto Lancaster Road/SR 37.", "Turn right.",
      "Turn right onto Lancaster Road (<span class=&quot;phoneme&quot;>/ˈlæŋkəstər ˈɹoʊd/</span>).",
      "Turn right onto Lancaster Road (<span class=&quot;phoneme&quot;>/ˈlæŋkəstər ˈɹoʊd/</span>), SR 37 (<span class=&quot;phoneme&quot;>/ˈsinjər 37/</span>).",
      "Continue for 400 meters.");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithExitSignPronunciation, CheckStreetNamesAndSigns2) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"G", "J"}, "auto");
  gurka::assert::raw::expect_path(result, {"I 70", "", "Lancaster Road/SR 37"});

  // Verify starting on I 70
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 1);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "I 70");

  // Verify sign pronunciations - alphabet & value
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations_size(),
            2);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .text(),
            "I 80");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .pronunciation()
                .alphabet(),
            Pronunciation_Alphabet_kIpa);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .pronunciation()
                .value(),
            "aɪ 80");

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .text(),
            "Main Street");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .pronunciation()
                .alphabet(),
            Pronunciation_Alphabet_kIpa);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .pronunciation()
                .value(),
            "meɪn strit");

  // Verify sign pronunciation instructions
  gurka::assert::raw::expect_instructions_at_maneuver_index(
      result, maneuver_index, "Take exit 126B toward I 80/Main Street.", "",
      "Take exit 126B (<span class=&quot;phoneme&quot;>/1 26bi/</span>).",
      "Take exit 126B (<span class=&quot;phoneme&quot;>/1 26bi/</span>) toward I 80 (<span class=&quot;phoneme&quot;>/aɪ 80/</span>), Main Street (<span class=&quot;phoneme&quot;>/meɪn strit/</span>).",
      "");
}