#pragma once

#include "data/tileData.h"
#include "pbf/pbf.hpp"

#include <vector>
#include <string>

namespace Tangram {

class Tile;

namespace PbfParser {

    struct ParserContext {
        std::vector<std::string> keys;
        std::vector<Value> values;
        std::vector<Properties::Item> properties;
        std::vector<protobuf::message> featureMsgs;
        std::vector<Point> coordinates;
        std::vector<int> numCoordinates;

        int tileExtent = 0;
    };
    
    void extractGeometry(ParserContext& _ctx, protobuf::message& _geomIn);
    
    void extractFeature(ParserContext& _ctx, protobuf::message& _featureIn, Feature& _out);
    
    void extractLayer(ParserContext& _ctx, protobuf::message& _in, Layer& _out);
    
    enum pbfGeomCmd {
        moveTo = 1,
        lineTo = 2,
        closePath = 7
    };
}

}
