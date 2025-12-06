#define CATCH_CONFIG_MAIN // Tells Catch2 to provide a main() function
#include <catch2/catch_all.hpp>
#include <fstream>
#include <string>

using namespace std;

// Change me or remove me to appropriately test part2 of this assignment
TEST_CASE("Test case 1 description goes here", "[tag1]")
{
	// Setup: An example using a resource file - Keep, change, or remove as needed.
	int x = 10;
	ifstream file("../resources/100ints.txt");
	REQUIRE(file.is_open());

	// Each SECTION runs independently, with fresh setup.
	SECTION("Section 1")
	{
		int y;
		file >> y;
		CHECK(y == -492);

		CHECK(x - 4 == 6.);
		CHECK(!false);
		CHECK(x == 10);
	}

	SECTION("Section 2")
	{
		int z;
		file >> z;
		CHECK(z == -492);

		CHECK_FALSE(false);
		CHECK(x != 1);
	}

	// Teardown
	file.close();
}

// Change me or remove me to appropriately test part2 of this assignment
TEST_CASE("Test case 2 description goes here", "[tag2]")
{
	CHECK(true);
}

// Change me or remove me to appropriately test part2 of this assignment
TEST_CASE("Test case 3 description goes here", "[tag2]")
{
	CHECK_FALSE(false);
}