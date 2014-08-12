#include <gtest/gtest.h>
#include <gtorrent/Core.hpp>

TEST(MiscTests, MagnetLink) {
	EXPECT_TRUE(gt::Core::isMagnetLink("magnet:"));
}
