export module alumi.syntax_tree.node_builder;

import alumi.parser.combinator;
import alumi.syntax_tree.nodes;
import alumi.parser.result;

export Node make_node(const Node::Type& node, const Result& res)
{
	return Node(node, res.get_subparser().start_token_index(), res.get_subparser().current_token_index());
} 
