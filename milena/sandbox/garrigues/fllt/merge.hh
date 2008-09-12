// Copyright (C) 2007, 2008 EPITA Research and Development Laboratory
//
// This file is part of the Olena Library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License version 2 as published by the
// Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this library; see the file COPYING.  If not, write to
// the Free Software Foundation, 51 Franklin Street, Fifth Floor,
// Boston, MA 02111-1307, USA.
//
// As a special exception, you may use this file as part of a free
// software library without restriction.  Specifically, if other files
// instantiate templates or use macros or inline functions from this
// file, or you compile this file and link it with other files to
// produce an executable, this file does not by itself cause the
// resulting executable to be covered by the GNU General Public
// License.  This exception does not however invalidate any other
// reasons why the executable file might be covered by the GNU General
// Public License.


#ifndef MLN_FIXME_FLLT_MERGE_HH
# define MLN_FIXME_FLLT_MERGE_HH

/*! \file merge.hh
 *
 * \brief Merge for Fast level line transform.
 *
 */

namespace mln
{
  namespace fllt
  {


    // Fwd declarations.
    template <typename P, typename V, typename F>
    void
    fill_a_shape(fllt_node(P, V)& node,
		 fllt_tree(P, V)& tree,
		 const image2d<fllt_node(P, V)*>& node_reg,
		 const image2d<fllt_node(P, V)*>& hole_reg);

    template <typename P, typename V, typename F>
    void
    move_shape(fllt_node(P, V)& node,
	       fllt_node(P, V)& hole,
	       fllt_tree(P, V)& tree,
	       const image2d<fllt_node(P, V)*>& hole_reg,
	       const image2d<fllt_node(P, V)*>& other_reg)
    {
      // FIXME : debug to remove.
      //      std::cout << "       [move_shape] "<< &hole << " as son of "<< &node << std::endl;
      //node.elt().points = set::uni(hole.elt().points, node.elt().points);
      node.add_child(&hole);
      fill_a_shape<P,V,typename F::opposite>(hole, tree, hole_reg, other_reg);
    }

    template <typename P, typename V, typename F>
    fllt_node(P, V)*
    find_the_hole(fllt_node(P, V)& node,
		  const P p,
		  const image2d<fllt_node(P, V)*>& other_reg)
    {
      fllt_node(P, V)* s = other_reg(p);
      mln_assertion(s);
      while (s->parent() && F::opposite::compare(s->parent()->elt().value, node.elt().value))
	//FIXME : Was while (s->parent() && (s->parent()->elt().value < node.elt().value))
      {
	mln_assertion(s);
	s = s->parent();
	mln_assertion(s);
      }
//       std::cout << "   [Find the hole] of " << p
// 		<< " from " << &node
// 		<< " return " << s
// 		<< std::endl;
      return s;
    }

    template <typename P, typename V, typename F>
    void
    fill_a_shape(fllt_node(P, V)& node,
		 fllt_tree(P, V)& tree,
		 const image2d<fllt_node(P, V)*>& node_reg,
		 const image2d<fllt_node(P, V)*>& hole_reg)
    {
//       std::cout << "[Start fill_a_shape] " << &node << " "
// 		<< node.elt().holes.nsites()
// 		<< " holes." << std::endl;

      if (node.elt().holes.nsites() == 0)
      {
	//	std::cout << "[End fill_a_shape]" << std::endl;
	return;
      }
      mln_piter(p_set<P>) p(node.elt().holes);
      for_all(p)
	{
	  bool h = true;

	  fllt_node(P, V)* hole;
	  if (node.elt().brighter == F::parent_is_brighter)
	    hole = find_the_hole<P,V,F>(node, point2d(p), hole_reg);
	  else
	    hole = find_the_hole<P,V,typename F::opposite>(node, point2d(p), node_reg);

	  mln_assertion(hole);

	  typename fllt_node(P, V)::children_t::iterator it;
	  for (it = node.children().begin();
	       it != node.children().end();
	       it++)
	  {
	    // Browse the hole of each child.
	    mln_piter(p_set<P>) q((*it)->elt().holes);
	    for_all(q)
	      {
		fllt_node(P, V)* child_hole = find_the_hole<P,V,F>((**it), point2d(q), hole_reg);
		if (set::is_subset_of(hole->elt().points,
				      child_hole->elt().points))

//		if (hole->elt().points < child_hole->elt().points)
		{
		  h = false;
		  break;
		}

	      }
	    if (!h)
	      break;
	  }
	  if (h)
	    move_shape<P,V,F>(node, *hole, tree, hole_reg, node_reg);
	}

      node.elt().holes.clear();
      //      std::cout << "[end fill_a_shape]" << std::endl;
    }

    template <typename P, typename V>
    fllt_tree(P, V)
      merge_trees(fllt_tree(P, V)& lower_tree,
		  fllt_tree(P, V)& upper_tree,
		  const image2d<fllt_node(P, V)*>& low_reg,
		  const image2d<fllt_node(P, V)*>& upp_reg,
		  const image2d<V>& ima)
    {

      //   In order to merge the trees, we only have to find for each shape S
      //   with a hole H in it whether one of its children has a hole H�
      //   containing H. If it is the case, we do nothing. Otherwise, we
      //   put the shape of the hole H (and all its descendants) as child of
      //   the shape .
      {
	std::cout << "[Merge first tree]------------" << std::endl;

	fllt_branch_iter_ind(P, V) p(lower_tree.main_branch());
	for_all(p)
	  {
	    fllt_node(P, V)& n(p);
	    fill_a_shape< P, V, lower<V> >(n, lower_tree, low_reg, upp_reg);
	    mln_assertion(lower_tree.check_consistency());
	    mln_assertion(upper_tree.check_consistency());
	  }

      }

      {
	std::cout << "[Merge second tree]------------" << std::endl;

	fllt_branch_iter_ind(P, V) p(upper_tree.main_branch());
	for_all(p)
	  {
	    fllt_node(P, V)& n(p);
	    fill_a_shape< P, V, upper<V> >(n, upper_tree, upp_reg, low_reg);
	    mln_assertion(lower_tree.check_consistency());
	    mln_assertion(upper_tree.check_consistency());
	  }
      }

      fllt_tree(P, V)* main_tree = &lower_tree;
      fllt_tree(P, V)* other_tree = &upper_tree;

      if (lower_tree.root()->elt().points.nsites() >= ima.domain().nsites())
      {
	main_tree = &upper_tree;
	other_tree = &lower_tree;
      }

      typename fllt_node(P, V)::children_t::iterator it;
      for (it = other_tree->root()->children().begin();
      	   it != other_tree->root()->children().end(); )
      {
      	main_tree->root()->add_child(*it);
      }
      mln_assertion(main_tree->check_consistency());
      return *main_tree;
    }


  } // end of namespace mln::fllt

} // end of namespace mln



#endif // ! MLN_FIXME_FLLT_MERGE_HH
