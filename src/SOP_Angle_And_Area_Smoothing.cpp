#include "SOP_Angle_And_Area_Smoothing.hpp"
#include "SOP_Angle_And_Area_Smoothing_Info.hpp"

#include <PRM/PRM_TemplateBuilder.h>

constexpr const char* DSFILE = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "rad"
        label   "Radius"
        type    vector2
        size    2           // 2 components in a vector2
        default { "1" "0.3" } // Outside and inside radius defaults
    }
}
)THEDSFILE";

PRM_Template* SOP_Angle_And_Area_Smoothing::buildTemplates()
{
    static PRM_TemplateBuilder templ(
            "SOP_Angle_And_Area_Smoothing.cpp"_sh,
            DSFILE
        );
    return templ.templates();
}

void newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_Angle_And_Area_Smoothing_Info::TYPENAME,   // Internal name
        SOP_Angle_And_Area_Smoothing_Info::NAME,                     // UI name
        SOP_Angle_And_Area_Smoothing::myConstructor,    // How to build the SOP
        SOP_Angle_And_Area_Smoothing::buildTemplates(), // My parameters
        1,                          // Min # of sources
        1,                          // Max # of sources
        nullptr,                    // Custom local variables (none)
        OP_FLAG_GENERATOR));        // Flag it as generator
}
