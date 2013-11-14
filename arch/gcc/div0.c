
/* Replacement (=dummy) for GNU/Linux division-by zero handler */
void __div0 (void)
{
  while (1)
    ;
}
