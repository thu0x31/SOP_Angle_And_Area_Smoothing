#include "SOP_Angle_And_Area_Smoothing.hpp"
#include "SOP_Angle_And_Area_Smoothing_Info.hpp"

#include <PRM/PRM_TemplateBuilder.h>

constexpr const char* DSFILE = R"THEDSFILE(
{
    name	parameters
    parm {
        name    "iteration"
        label   "iteration"
        type    integer
        default { "0" }
        range   { 1 50 }
        parmtag { "script_callback_language" "python" }
    }
    parm {
        name    "safetyConstraints"
        label   "safetyConstraints"
        type    toggle
        default { "0" }
        parmtag { "script_callback_language" "python" }
    }
    parm {
        name    "useAngleSmoothing"
        label   "useAngleSmoothing"
        type    toggle
        default { "0" }
        parmtag { "script_callback_language" "python" }
    }
    parm {
        name    "useAreaSmoothing"
        label   "useAreaSmoothing"
        type    toggle
        default { "0" }
        parmtag { "script_callback_language" "python" }
    }
    parm {
        name    "useDelaunayflips"
        label   "useDelaunayflips"
        type    toggle
        default { "0" }
        parmtag { "script_callback_language" "python" }
    }
    groupsimple {
        name    "constrainedEdge"
        label   "constrainedEdge"
        grouptag { "group_type" "simple" }

        parm {
            name    "useConstrainedEdge"
            label   "use"
            type    toggle
            default { "0" }
            parmtag { "script_callback_language" "python" }
        }
        parm {
            name    "angle"
            label   "angle"
            type    float
            default { "0" }
            disablewhen "{ useConstrainedEdge == 0 }"
            range   { -360 360 }
            parmtag { "script_callback_language" "python" }
        }
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
