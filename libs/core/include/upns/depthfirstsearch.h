/*******************************************************************************
 *
 * Copyright 2016-2017 Daniel Bulla	<d.bulla@fh-aachen.de>
 *           2017-2018 Tobias Neumann	<t.neumann@fh-aachen.de>
 *
******************************************************************************/

/*  This file is part of mapit.
 *
 *  Mapit is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Mapit is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with mapit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DEPTHFIRSTSEARCH_H
#define DEPTHFIRSTSEARCH_H

#include <upns/typedefs.h>
#include <upns/logging.h>
#include <mapit/msgs/services.pb.h>
#include <upns/versioning/checkout.h>

namespace upns
{
/**
 * @brief Depth first search for Commit, Tree and Entity. This is often very handy.
 * Does not work for branches. Does not visit Entitydata (must be done manually).
 * If "before" returns false, "after" will not be executed.
 */
StatusCode depthFirstSearch(CheckoutCommon *checkout, std::shared_ptr<mapit::msgs::Commit> obj, const ObjectReference &ref, const Path& path,
                            std::function<bool(std::shared_ptr<mapit::msgs::Commit>, const ObjectReference&, const Path&)> beforeCommit, std::function<bool(std::shared_ptr<mapit::msgs::Commit>, const ObjectReference&, const Path&)> afterCommit,
                            std::function<bool(std::shared_ptr<mapit::msgs::Tree>, const ObjectReference&, const Path&)> beforeTree, std::function<bool(std::shared_ptr<mapit::msgs::Tree>, const ObjectReference&, const Path&)> afterTree,
                            std::function<bool(std::shared_ptr<mapit::msgs::Entity>, const ObjectReference&, const Path&)> beforeEntity, std::function<bool(std::shared_ptr<mapit::msgs::Entity>, const ObjectReference&, const Path&)> afterEntity);

StatusCode depthFirstSearch(CheckoutCommon *checkout, std::shared_ptr<mapit::msgs::Tree> obj, const ObjectReference &ref, const Path& path,
                            std::function<bool(std::shared_ptr<mapit::msgs::Commit>, const ObjectReference&, const Path&)> beforeCommit, std::function<bool(std::shared_ptr<mapit::msgs::Commit>, const ObjectReference&, const Path&)> afterCommit,
                            std::function<bool(std::shared_ptr<mapit::msgs::Tree>, const ObjectReference&, const Path&)> beforeTree, std::function<bool(std::shared_ptr<mapit::msgs::Tree>, const ObjectReference&, const Path&)> afterTree,
                            std::function<bool(std::shared_ptr<mapit::msgs::Entity>, const ObjectReference&, const Path&)> beforeEntity, std::function<bool(std::shared_ptr<mapit::msgs::Entity>, const ObjectReference&, const Path&)> afterEntity);

StatusCode depthFirstSearch(CheckoutCommon *checkout, std::shared_ptr<mapit::msgs::Entity> obj, const ObjectReference &ref, const Path& path,
                            std::function<bool(std::shared_ptr<mapit::msgs::Commit>, const ObjectReference&, const Path&)> beforeCommit, std::function<bool(std::shared_ptr<mapit::msgs::Commit>, const ObjectReference&, const Path&)> afterCommit,
                            std::function<bool(std::shared_ptr<mapit::msgs::Tree>, const ObjectReference&, const Path&)> beforeTree, std::function<bool(std::shared_ptr<mapit::msgs::Tree>, const ObjectReference&, const Path&)> afterTree,
                            std::function<bool(std::shared_ptr<mapit::msgs::Entity>, const ObjectReference&, const Path&)> beforeEntity, std::function<bool(std::shared_ptr<mapit::msgs::Entity>, const ObjectReference&, const Path&)> afterEntity);

StatusCode depthFirstSearch(  upns::CheckoutCommon *checkout
                            , std::function<bool(std::shared_ptr<Commit>, const ObjectReference&, const Path&)> beforeCommit
                            , std::function<bool(std::shared_ptr<Commit>, const ObjectReference&, const Path&)> afterCommit
                            , std::function<bool(std::shared_ptr<Tree>, const ObjectReference&, const Path&)> beforeTree
                            , std::function<bool(std::shared_ptr<Tree>, const ObjectReference&, const Path&)> afterTree
                            , std::function<bool(std::shared_ptr<Entity>, const ObjectReference&, const Path&)> beforeEntity
                            , std::function<bool(std::shared_ptr<Entity>, const ObjectReference&, const Path&)> afterEntity);

StatusCode depthFirstSearch(  upns::CheckoutCommon *checkout
                            , const Path& path
                            , std::function<bool(std::shared_ptr<Commit>, const ObjectReference&, const Path&)> beforeCommit
                            , std::function<bool(std::shared_ptr<Commit>, const ObjectReference&, const Path&)> afterCommit
                            , std::function<bool(std::shared_ptr<Tree>, const ObjectReference&, const Path&)> beforeTree
                            , std::function<bool(std::shared_ptr<Tree>, const ObjectReference&, const Path&)> afterTree
                            , std::function<bool(std::shared_ptr<Entity>, const ObjectReference&, const Path&)> beforeEntity
                            , std::function<bool(std::shared_ptr<Entity>, const ObjectReference&, const Path&)> afterEntity);

#define depthFirstSearchAll(c) ([](std::shared_ptr<c> obj, const ObjectReference& ref, const upns::Path &path){return true;})

}

#endif
