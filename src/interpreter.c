#define OP_START      0xFFFC
#define SP_START      0xFA00

typedef struct INTERPRETER
{
	u32 Segment;
	u16 SP, /* Stack pointer */
		FP, /* Frame pointer */
		IP, /* Instruction pointer */
		OP; /* Operand pointer */
} Interpreter;

#define COLOR_RED    "\033[1;31m"
#define COLOR_PURPLE "\033[1;35m"
#define COLOR_RESET  "\033[0m"

static u8 _debug_instruction(Interpreter *i)
{
	u8 size = 0;
	printf("\n         %5d | %04X | " COLOR_RED, i->IP, i->IP);
	switch(memory_r8(i->Segment + i->IP))
	{
	case INSTR_HALT:
		printf("HALT\n");
		size = 1;
		break;

	case INSTR_PUSHI8:
		printf("PUSHI8 %d\n", memory_r8(i->Segment + i->IP + 1));
		size = 2;
		break;

	case INSTR_PUSHI16:
		printf("PUSHI16 %d\n", memory_r16(i->Segment + i->IP + 1));
		size = 3;
		break;

	case INSTR_PUSHI32:
		printf("PUSHI32 %d\n", memory_r32(i->Segment + i->IP + 1));
		size = 5;
		break;

	case INSTR_PUSHL:
		printf("PUSHL %d\n", memory_r8(i->Segment + i->IP + 1));
		size = 2;
		break;

	case INSTR_POPL:
		printf("POPL %d\n", memory_r8(i->Segment + i->IP + 1));
		size = 2;
		break;

	case INSTR_PUSHG:
		printf("PUSHG %d\n", memory_r8(i->Segment + i->IP + 1));
		size = 2;
		break;

	case INSTR_POPG:
		printf("POPG %d\n", memory_r8(i->Segment + i->IP + 1));
		size = 2;
		break;

	case INSTR_POP:
		printf("POP\n");
		size = 1;
		break;

	case INSTR_JT:
		{
			u8 j, cnt;
			cnt = memory_r8(i->Segment + i->IP + 1);
			printf("JT %d: ", cnt);
			for(j = 0; j < cnt; ++j)
			{
				printf("%d%c ", memory_r16(i->Segment + i->IP + 2 + 2 * j), j < cnt - 1 ? ',' : '\n');
			}

			size = 2 + 2 * cnt;
		}
		break;

	case INSTR_JZ:
		printf("JZ %d\n", memory_r16(i->Segment + i->IP + 1));
		size = 3;
		break;

	case INSTR_JNZ:
		printf("JNZ %d\n", memory_r16(i->Segment + i->IP + 1));
		size = 3;
		break;

	case INSTR_JMP:
		printf("JMP %d\n", memory_r16(i->Segment + i->IP + 1));
		size = 3;
		break;

	case INSTR_CALL:
		{
			u8 args = memory_r8(i->Segment + i->IP + 1);
			i16 addr = memory_r16(i->Segment + i->IP + 2);
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
		printf("DSP %d\n", memory_r8(i->Segment + i->IP + 1));
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

static void _disasm(Interpreter *i, u16 len)
{
	u16 prev;
	prev = i->IP;
	while(i->IP < len)
	{
		i->IP += _debug_instruction(i);
	}

	i->IP = prev;
}

#ifdef DEBUG_INTERPRETER

static void _debug_stack(Interpreter *i)
{
	u32 addr, start, end, slot;

	start = i->FP + 32;
	end = i->SP - 32;

	if(start > SP_START)
	{
		start = SP_START;
	}

	slot = (SP_START - start) / 4;

	printf("+------+-------+------+------------+----------+\n");
	for(addr = start; addr >= end; addr -= 4, ++slot)
	{
		u32 value = memory_r32(addr);

		printf("| %4d | %5d | %04X | %10d | %08X |", slot, addr, addr, value, value);

		if(i->FP == addr || i->SP == addr)
		{
			printf(COLOR_PURPLE);
		}

		if(i->FP == addr)
		{
			printf(" <-- FP ");
		}

		if(i->SP == addr)
		{
			printf(" <-- SP ");
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
		u32 value = memory_r32(addr);

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

static void interpreter_init(Interpreter *i, u32 segment)
{
	/* Empty descending stack */
	i->Segment = segment;
	i->OP = OP_START;
	i->SP = SP_START;
	i->FP = i->SP;
	i->IP = 0;

#ifdef DEBUG_INTERPRETER
	_debug_stack(i);
	_debug_op_stack(i);
#endif
}

static i8 interpreter_step(Interpreter *i)
{
#ifdef DEBUG_INTERPRETER
	_debug_instruction(i);
#endif

	switch(memory_r8(i->Segment + i->IP))
	{
	case INSTR_HALT:
		/* Finish program */
		return 1;

	case INSTR_PUSHI8:
		/* Push 8-bit immediate value to the stack */
		i->IP += 1;
		memory_w32(i->Segment + i->OP, memory_r8(i->Segment + i->IP));
		i->OP -= 4;
		i->IP += 1;
		break;

	case INSTR_PUSHI16:
		/* Push 16-bit immediate value to the stack */
		i->IP += 1;
		memory_w32(i->Segment + i->OP, memory_r16(i->Segment + i->IP));
		i->OP -= 4;
		i->IP += 2;
		break;

	case INSTR_PUSHI32:
		/* Push 32-bit immediate value to the stack */
		i->IP += 1;
		memory_w32(i->Segment + i->OP, memory_r32(i->Segment + i->IP));
		i->OP -= 4;
		i->IP += 4;
		break;

	case INSTR_PUSHL:
		/* Push value from local variable to stack */
		{
			u16 offset;
			i->IP += 1;
			offset = 4 * memory_r8(i->Segment + i->IP);
			memory_w32(i->Segment + i->OP, memory_r32(i->Segment + i->FP - offset));
			i->OP -= 4;
			i->IP += 1;
		}
		break;

	case INSTR_POPL:
		/* Pop value from stack to local variable */
		{
			u16 offset;
			i->IP += 1;
			offset = 4 * memory_r8(i->Segment + i->IP);
			i->OP += 4;
			memory_w32(i->Segment + i->FP - offset, memory_r32(i->Segment + i->OP));
			i->IP += 1;
		}
		break;

	case INSTR_PUSHG:
		/* Push value from global variable to stack */
		{
			u16 offset;
			i->IP += 1;
			offset = 4 * memory_r8(i->Segment + i->IP);
			memory_w32(i->Segment + i->OP, memory_r32(i->Segment + SP_START - offset));
			i->OP -= 4;
			i->IP += 1;
		}
		break;

	case INSTR_POPG:
		/* Pop value from stack to global variable */
		{
			u16 offset;
			i->IP += 1;
			offset = 4 * memory_r8(i->Segment + i->IP);
			i->OP += 4;
			memory_w32(i->Segment + SP_START - offset, memory_r32(i->Segment + i->OP));
			i->IP += 1;
		}
		break;

	case INSTR_JT:
		{
			u32 v;
			u8 cnt;

			i->OP += 4;
			v = memory_r32(i->Segment + i->OP);

			i->IP += 1;
			cnt = memory_r8(i->Segment + i->IP);

			if(v > cnt)
			{
				return -1;
			}

			i->IP += 1;
			i->IP = memory_r16(i->Segment + i->IP + 2 * v);
		}
		break;

	case INSTR_JZ:
		/* Jump to fixed location if stack top is zero */
		i->OP += 4;
		if(memory_r32(i->Segment + i->OP))
		{
			i->IP += 3;
		}
		else
		{
			i->IP = memory_r16(i->Segment + i->IP + 1);
		}
		break;

	case INSTR_JNZ:
		/* Jump to fixed location if stack top is not zero */
		i->OP += 4;
		if(memory_r32(i->Segment + i->OP))
		{
			i->IP = memory_r16(i->Segment + i->IP + 1);
		}
		else
		{
			i->IP += 3;
		}
		break;

	case INSTR_JMP:
		/* Unconditional jump to fixed location */
		i->IP += 1;
		i->IP = memory_r16(i->Segment + i->IP);
		break;

	case INSTR_CALL:
		/* Call function */
		{
			u8 args, j;
			i16 func;

			/* Get function args */
			i->IP += 1;
			args = memory_r8(i->Segment + i->IP);

			/* Get function address */
			i->IP += 1;
			func = memory_r16(i->Segment + i->IP);

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
					arg_buf[j] = memory_r32(i->Segment + i->OP - 4 * j);
				}

				/* Call function id with parameter offset */
				ret = _builtin_call(-func - 1, arg_buf);

				/* Return Value */
				memory_w32(i->Segment + i->OP, ret);
				i->OP -= 4;
			}
			else
			{
				/* Store return address on stack */
				memory_w32(i->Segment + i->SP, i->IP);
				i->SP -= 4;

				/* Store frame pointer of caller on stack */
				memory_w32(i->Segment + i->SP, i->FP);
				i->SP -= 4;

				/* Set frame pointer of callee to top of stack */
				i->FP = i->SP;

				/* Copy Paramters */
				for(j = 0; j < args; ++j)
				{
					i->OP += 4;
					memory_w32(i->Segment + i->SP - 4 * ((args - 1) - j), memory_r32(i->Segment + i->OP));
				}

				/* Jump to function */
				i->IP = func;
			}
		}
		break;

	case INSTR_RET:
		/* Return from function */
		i->SP = i->FP + 4;
		i->FP = memory_r32(i->Segment + i->SP);
		i->SP += 4;
		i->IP = memory_r32(i->Segment + i->SP);
		break;

	case INSTR_DSP:
		/* Decrement stack pointer by fixed amount */
		i->IP += 1;
		i->SP -= 4 * memory_r8(i->Segment + i->IP);
		i->IP += 1;
		break;

	case INSTR_POP:
		/* Remove the top element from the stack */
		i->IP += 1;
		i->OP += 4;
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
	return 0;
}
