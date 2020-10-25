//
// Created by tobias on 08.11.19.
//
#pragma once

#ifndef DBGTTHESIS_TYPEDEFS_H
#define DBGTTHESIS_TYPEDEFS_H
/**
 * typedefs for the decomposition tree
 */
namespace decomposition_tree {
    enum STEP_ACTION {
        DELETION, MIGRATION, REQUEST, SKIPPED, NONE
    };
    typedef unsigned int node_;
    typedef unsigned int weight_;
    typedef unsigned int id_;
}

#endif //DBGTTHESIS_TYPEDEFS_H
