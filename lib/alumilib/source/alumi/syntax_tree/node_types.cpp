import alumi.syntax_tree;

namespace alumi:syntax_tree
{
Error::Error(parser::ErrorCode ec, size_t token_index, const Nodes& children)
	: groups({ ChildGroup(1, children.size()) })
	, error_code(ec)
	, token_index(token_index)
{

}

ModuleRoot::ModuleRoot(const Nodes& children)
	: groups({ ChildGroup(1, children.size()) })
{

}

CodeBlock::CodeBlock(const Nodes& children)
	: groups({ ChildGroup(1, children.size()) })
{

}

Assignment::Assignment(const Node& value)
	: groups({ ChildGroup(1, value.recursive_child_count()) })
{

}


Statement::Statement(const Nodes& children)
	: groups({ ChildGroup(1, children.size()) })
{

}

FunctionDecleration::FunctionDecleration(const Node& params, const std::optional<Node>& return_type)
	: groups({ ChildGroup(1, params.recursive_child_count()), 
			      ChildGroup(1 + params.recursive_child_count(), return_type != std::nullopt ? return_type->recursive_child_count() : 0)})
{

}


FunctionDefinition::FunctionDefinition(const Node& decleration, const Node& code)
	: groups({ ChildGroup(1, decleration.recursive_child_count()),
					ChildGroup(1 + decleration.recursive_child_count(), code.recursive_child_count()) })
{

}

}
