import { render, screen } from "@testing-library/react";
import { vi } from "vitest";
import { Authorization } from "@/pages";

vi.mock("@/components", () => ({
  AuthLayout: ({ children }: any) => (
    <div data-testid="layout">{children}</div>
  ),
  AuthForm: ({ mode }: any) => (
    <div data-testid="form">FORM_MODE: {mode}</div>
  ),
}));

describe("Authorization component", () => {
  it("renders the layout and the form", () => {
    render(<Authorization mode="login" />);

    expect(screen.getByTestId("layout")).toBeInTheDocument();
    expect(screen.getByTestId("form")).toBeInTheDocument();
  });

  it("passes the correct mode to AuthForm (login)", () => {
    render(<Authorization mode="login" />);

    expect(screen.getByText("FORM_MODE: login")).toBeInTheDocument();
  });

  it("passes the correct mode to AuthForm (register)", () => {
    render(<Authorization mode="register" />);

    expect(screen.getByText("FORM_MODE: register")).toBeInTheDocument();
  });
});


