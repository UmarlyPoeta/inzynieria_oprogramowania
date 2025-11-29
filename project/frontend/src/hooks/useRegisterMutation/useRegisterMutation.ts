import { useMutation, useQueryClient } from "@tanstack/react-query";
import { SignUp } from "@/api/auth";

const useRegisterMutation = () => {
  const qc = useQueryClient();

  return useMutation({
    mutationFn: SignUp,
    onSuccess: (data) => {
      localStorage.setItem("token", data.token);
      qc.invalidateQueries({ queryKey: ["auth", "user"] });
    },
  });
};

export default useRegisterMutation;
