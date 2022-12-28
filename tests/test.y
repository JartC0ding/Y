$id = (λx. x)
$five = 5

$id($five)

// id of id of id of 6 -> 6
(λx. x)((λy. y)((λz. z)(6)))

// id of id
(λx. x)((λy. y))

