#define OP_START      0xFFFC
#define CSP_START      0xFA00

typedef struct _INTERPRETER
{
	u16 CSP, /* Call stack pointer */
		FP, /* Frame pointer */
		IP, /* Instruction pointer */
		OP; /* Operand pointer */
} Interpreter;

#ifdef DEBUG_INTERPRETER

static u8 _debug_instruction(Interpreter *i)
{
	u8 size = 0;
	printf("\n         %5d | %04X | " COLOR_RED, i->IP, i->IP);
	switch(memory_r8(BANK_INTERPRETER, i->IP))
	{
	case INSTR_HALT:
		printf("HALT\n");
		size = 1;
		break;

	case INSTR_PUSHI8:
		printf("PUSHI8 %d\n", memory_r8(BANK_INTERPRETER, i->IP + 1));
		size = 2;
		break;

	case INSTR_PUSHI16:
		printf("PUSHI16 %d\n", memory_r16(BANK_INTERPRETER, i->IP + 1));
		size = 3;
		break;

	case INSTR_PUSHI32:
		printf("PUSHI32 %d\n", memory_r32(BANK_INTERPRETER, i->IP + 1));
		size = 5;
		break;

	case INSTR_PUSHL:
		printf("PUSHL %d\n", memory_r8(BANK_INTERPRETER, i->IP + 1));
		size = 2;
		break;

	case INSTR_POPL:
		printf("POPL %d\n", memory_r8(BANK_INTERPRETER, i->IP + 1));
		size = 2;
		break;

	case INSTR_PUSHG:
		printf("PUSHG %d\n", memory_r8(BANK_INTERPRETER, i->IP + 1));
		size = 2;
		break;

	case INSTR_POPG:
		printf("POPG %d\n", memory_r8(BANK_INTERPRETER, i->IP + 1));
		size = 2;
		break;

	case INSTR_POP:
		printf("POP\n");
		size = 1;
		break;

	case INSTR_DUP:
		printf("DUP\n");
		size = 1;
		break;

	case INSTR_JT:
		{
			u8 j, cnt;
			cnt = memory_r8(BANK_INTERPRETER, i->IP + 1);
			printf("JT %d: ", cnt);
			for(j = 0; j < cnt; ++j)
			{
				printf("%d%c ", memory_r16(BANK_INTERPRETER, i->IP + 2 + 2 * j), j < cnt - 1 ? ',' : '\n');
			}

			size = 2 + 2 * cnt;
		}
		break;

	case INSTR_JZ:
		printf("JZ %d\n", memory_r16(BANK_INTERPRETER, i->IP + 1));
		size = 3;
		break;

	case INSTR_JNZ:
		printf("JNZ %d\n", memory_r16(BANK_INTERPRETER, i->IP + 1));
		size = 3;
		break;

	case INSTR_JMP:
		printf("JMP %d\n", memory_r16(BANK_INTERPRETER, i->IP + 1));
		size = 3;
		break;

	case INSTR_CALL:
		{
			u8 args = memory_r8(BANK_INTERPRETER, i->IP + 1);
			i16 addr = memory_r16(BANK_INTERPRETER, i->IP + 2);
			if(addr < 0)
			{
				printf("CALL %s\n", _builtin_name(-addr - 1));
			}
			else
			{
				printf("CALL %d (%d)\n", addr, args);
			}
		}
		size = 4;
		break;

	case INSTR_RET:
		printf("RET\n");
		size = 1;
		break;

	case INSTR_DSP:
		printf("DSP %d\n", memory_r8(BANK_INTERPRETER, i->IP + 1));
		size = 2;
		break;

	default:
		printf("INVALID INSTRUCTION\n");
		size = 1;
		break;
	}

	printf(COLOR_RESET);
	return size;
}

static void disasm(Interpreter *i, u16 len)
{
	u16 prev;

	printf("DISASM!\n");

	prev = i->IP;
	while(i->IP < len)
	{
		i->IP += _debug_instruction(i);
	}
	printf("END!\n");

	i->IP = prev;
}

static void _debug_stack(Interpreter *i)
{
	u32 addr, start, end, slot;

	start = i->FP + 32;
	end = i->CSP - 32;

	if(start > CSP_START)
	{
		start = CSP_START;
	}

	slot = (CSP_START - start) / 4;

	printf("+------+-------+------+------------+----------+\n");
	for(addr = start; addr >= end; addr -= 4, ++slot)
	{
		u32 value = memory_r32(BANK_INTERPRETER, addr);

		printf("| %4d | %5d | %04X | %10d | %08X |", slot, addr, addr, value, value);

		if(i->FP == addr || i->CSP == addr)
		{
			printf(COLOR_PURPLE);
		}

		if(i->FP == addr)
		{
			printf(" <-- FP ");
		}

		if(i->CSP == addr)
		{
			printf(" <-- CSP ");
		}

		printf(COLOR_RESET "\n");
	}

	printf("+------+-------+------+------------+----------+\n");
}

static void _debug_op_stack(Interpreter *i)
{
	u32 addr, start, end, slot;

	start = i->OP + 32;
	end = i->OP - 32;

	if(start > OP_START)
	{
		start = OP_START;
	}

	slot = (OP_START - start) / 4;

	printf("+------+-------+------+------------+----------+\n");
	for(addr = start; addr >= end; addr -= 4, ++slot)
	{
		u32 value = memory_r32(BANK_INTERPRETER, addr);

		printf("| %4d | %5d | %04X | %10d | %08X |", slot, addr, addr, value, value);

		if(i->OP == addr)
		{
			printf(COLOR_PURPLE);
		}

		if(i->OP == addr)
		{
			printf(" <-- OP ");
		}

		printf(COLOR_RESET "\n");
	}

	printf("+------+-------+------+------------+----------+\n");
}

#endif

static void interpreter_init(Interpreter *i)
{
	/* Empty descending stack */
	i->OP = OP_START;
	i->CSP = CSP_START;
	i->FP = i->CSP;
	i->IP = 0;

#ifdef DEBUG_INTERPRETER
	_debug_stack(i);
	_debug_op_stack(i);
#endif
}

static StatusCode interpreter_step(Interpreter *i)
{
#ifdef DEBUG_INTERPRETER
	_debug_instruction(i);
#endif

	switch(memory_r8(BANK_INTERPRETER, i->IP))
	{
	case INSTR_HALT:
		/* Finish program */
		return 1;

	case INSTR_PUSHI8:
		/* Push 8-bit immediate value to the stack */
		i->IP += 1;
		memory_w32(BANK_INTERPRETER, i->OP, memory_r8(BANK_INTERPRETER, i->IP));
		i->OP -= 4;
		i->IP += 1;
		break;

	case INSTR_PUSHI16:
		/* Push 16-bit immediate value to the stack */
		i->IP += 1;
		memory_w32(BANK_INTERPRETER, i->OP, memory_r16(BANK_INTERPRETER, i->IP));
		i->OP -= 4;
		i->IP += 2;
		break;

	case INSTR_PUSHI32:
		/* Push 32-bit immediate value to the stack */
		i->IP += 1;
		memory_w32(BANK_INTERPRETER, i->OP, memory_r32(BANK_INTERPRETER, i->IP));
		i->OP -= 4;
		i->IP += 4;
		break;

	case INSTR_PUSHL:
		/* Push value from local variable to stack */
		{
			u16 offset;
			i->IP += 1;
			offset = 4 * memory_r8(BANK_INTERPRETER, i->IP);
			memory_w32(BANK_INTERPRETER, i->OP, memory_r32(BANK_INTERPRETER, i->FP - offset));
			i->OP -= 4;
			i->IP += 1;
		}
		break;

	case INSTR_POPL:
		/* Pop value from stack to local variable */
		{
			u16 offset;
			i->IP += 1;
			offset = 4 * memory_r8(BANK_INTERPRETER, i->IP);
			i->OP += 4;
			memory_w32(BANK_INTERPRETER, i->FP - offset, memory_r32(BANK_INTERPRETER, i->OP));
			i->IP += 1;
		}
		break;

	case INSTR_PUSHG:
		/* Push value from global variable to stack */
		{
			u16 offset;
			i->IP += 1;
			offset = 4 * memory_r8(BANK_INTERPRETER, i->IP);
			memory_w32(BANK_INTERPRETER, i->OP, memory_r32(BANK_INTERPRETER, CSP_START - offset));
			i->OP -= 4;
			i->IP += 1;
		}
		break;

	case INSTR_POPG:
		/* Pop value from stack to global variable */
		{
			u16 offset;
			i->IP += 1;
			offset = 4 * memory_r8(BANK_INTERPRETER, i->IP);
			i->OP += 4;
			memory_w32(BANK_INTERPRETER, CSP_START - offset, memory_r32(BANK_INTERPRETER, i->OP));
			i->IP += 1;
		}
		break;

	case INSTR_JT:
		{
			u32 v;
			u8 cnt;

			i->OP += 4;
			v = memory_r32(BANK_INTERPRETER, i->OP);

			i->IP += 1;
			cnt = memory_r8(BANK_INTERPRETER, i->IP);

			if(v > cnt)
			{
				return -1;
			}

			i->IP += 1;
			i->IP = memory_r16(BANK_INTERPRETER, i->IP + 2 * v);
		}
		break;

	case INSTR_JZ:
		/* Jump to fixed location if stack top is zero */
		i->OP += 4;
		if(memory_r32(BANK_INTERPRETER, i->OP))
		{
			i->IP += 3;
		}
		else
		{
			i->IP = memory_r16(BANK_INTERPRETER, i->IP + 1);
		}
		break;

	case INSTR_JNZ:
		/* Jump to fixed location if stack top is not zero */
		i->OP += 4;
		if(memory_r32(BANK_INTERPRETER, i->OP))
		{
			i->IP = memory_r16(BANK_INTERPRETER, i->IP + 1);
		}
		else
		{
			i->IP += 3;
		}
		break;

	case INSTR_JMP:
		/* Unconditional jump to fixed location */
		i->IP += 1;
		i->IP = memory_r16(BANK_INTERPRETER, i->IP);
		break;

	case INSTR_CALL:
		/* Call function */
		{
			u8 args, j;
			i16 func;

			/* Get function args */
			i->IP += 1;
			args = memory_r8(BANK_INTERPRETER, i->IP);

			/* Get function address */
			i->IP += 1;
			func = memory_r16(BANK_INTERPRETER, i->IP);

			/* Next instruction address */
			i->IP += 2;

			/* Negative address: builtin function trap */
			if(func < 0)
			{
				/* Builtin function */
				u32 ret, arg_buf[args];

				/* Copy Paramters */
				i->OP += 4 * args;
				for(j = 0; j < args; ++j)
				{
					arg_buf[j] = memory_r32(BANK_INTERPRETER, i->OP - 4 * j);
				}

				/* Call function id with parameter offset */
				ret = _builtin_call(-func - 1, arg_buf);

				/* Return Value */
				memory_w32(BANK_INTERPRETER, i->OP, ret);
				i->OP -= 4;
			}
			else
			{
				/* Store return address on stack */
				memory_w32(BANK_INTERPRETER, i->CSP, i->IP);
				i->CSP -= 4;

				/* Store frame pointer of caller on stack */
				memory_w32(BANK_INTERPRETER, i->CSP, i->FP);
				i->CSP -= 4;

				/* Set frame pointer of callee to top of stack */
				i->FP = i->CSP;

				/* Copy Paramters */
				for(j = 0; j < args; ++j)
				{
					i->OP += 4;
					memory_w32(BANK_INTERPRETER, i->CSP - 4 * ((args - 1) - j), memory_r32(BANK_INTERPRETER, i->OP));
				}

				/* Jump to function */
				i->IP = func;
			}
		}
		break;

	case INSTR_RET:
		/* Return from function */
		i->CSP = i->FP + 4;
		i->FP = memory_r32(BANK_INTERPRETER, i->CSP);
		i->CSP += 4;
		i->IP = memory_r32(BANK_INTERPRETER, i->CSP);
		break;

	case INSTR_DSP:
		/* Decrement stack pointer by fixed amount */
		i->IP += 1;
		i->CSP -= 4 * memory_r8(BANK_INTERPRETER, i->IP);
		i->IP += 1;
		break;

	case INSTR_POP:
		/* Remove the top element from the stack */
		i->IP += 1;
		i->OP += 4;
		break;

	case INSTR_DUP:
		{
			/* Duplicate top element on the stack */
			u32 value;
			i->IP += 1;
			value = memory_r32(BANK_INTERPRETER, i->OP + 4);
			memory_w32(BANK_INTERPRETER, i->OP, value);
			i->OP -= 4;
		}
		break;

	default:
		/* Invalid instruction */
		return -1;
	}

#ifdef DEBUG_INTERPRETER
	_debug_stack(i);
	_debug_op_stack(i);
#endif

	/* Success, next instruction */
	return SUCCESS;
}
