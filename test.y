$call = (\x y. x(y))
$id = (λx. x)
$five = 5

$id($id)

// id of id of id of 6 -> 6
(λx. x)((λy. y)((λz. z)(6)))
$call($id 5)

// id of id
(λx. x)((λy. y))
